#include "fw_driver_bq76972.h"

#include <stdio.h>

#include "hardware/spi.h"
#include "pico/stdlib.h"

// ============================================================================
// Configuration
// ============================================================================

#define SPI_PORT spi0
#define PIN_RX 0
#define PIN_CS 1
#define PIN_SCK 2
#define PIN_TX 3

#define SPI_BAUDRATE (250 * 1000)
#define BITS_PER_TRANSFER 8
#define SPI_CPOL 0
#define SPI_CPHA 0
#define SPI_ORDER 1

#define SPI_TRANSACTION_DELAY_US 2000
#define SPI_MAX_RETRIES 10
#define WAKEUP_DELAY_MS 5
#define SPI_RETRY_BACKOFF_US 2000

#define SUBCOMMAND_LOWER 0x3E
#define SUBCOMMAND_UPPER 0x3F
#define RESPONSE_BUFFER_START 0x40
#define RESPONSE_BUFFER_CHKSUM 0x60
#define RESPONSE_BUFFER_LENGTH 0x61

#define DATA_MEMORY_MAX_BYTES 32
#define DATA_MEMORY_RESPONSE_DELAY_US 10000
#define CONFIG_SETTLE_DELAY_MS 100

// Data memory addresses for pin configuration
#define DFETOFF_CONFIG_ADDR 0x92FB
#define TS1_CONFIG_ADDR 0x92FD
#define TS2_CONFIG_ADDR 0x92FE
#define DA_CONFIG_ADDR 0x9303
#define TS3_CONFIG_ADDR 0x92FF
#define DCHG_CONFIG_ADDR 0x9301
#define DDSG_CONFIG_ADDR 0x9302

// For dedicated TS pins (TS1, TS2, TS3):
//   PIN_FXN[2:0] = 0b111 = thermistor mode
//   OPT[1:0] (bits [4:3]) = 00 = used for protections, 18K temperature model
//   OPT[3:2] (bits [6:5]) = 00 = 18K pull-up
// 0x07 matches TI's reference code for cell temperature thermistors.
#define THERMISTOR_CONFIG_TS_PIN 0x07

// For multifunction pins (DFETOFF, DCHG, DDSG):
//   PIN_FXN[2:0] = 0b011 = ADC input / thermistor mode
//   OPT[1:0] (bits [4:3]) = 01 = thermistor, reported but not used for protections
//   OPT[3:2] (bits [6:5]) = 00 = 18K temperature model
//   OPT[5:4] (bits [8:7]) = 00 = 18K pull-up
#define THERMISTOR_CONFIG_MULTIFUNCTION_PIN 0x0B

#define THERMISTOR_TS_DEFAULT_CONFIG THERMISTOR_CONFIG_TS_PIN
#define THERMISTOR_MF_DEFAULT_CONFIG THERMISTOR_CONFIG_MULTIFUNCTION_PIN

#define DEBUG 0

// Cached USER_AMPS setting — updated by fw_bq76972_configure_user_amps()
static fw_user_amps_t cached_user_amps = FW_USER_AMPS_1_MA;

// ============================================================================
// Private Helper Functions
// ============================================================================

static void print_hex(uint8_t data[], size_t length)
{
    if (!DEBUG)
    {
        return;
    }

    printf("0x");
    for (size_t i = 0; i < length; i++)
    {
        printf("%02X", data[i]);
    }
    printf("\n");
}

static uint8_t calculate_crc(uint8_t data[], size_t length)
{
    uint8_t crc = 0;

    for (size_t i = 0; i < length; i++)
    {
        crc ^= data[i];
        for (int j = 0; j < 8; j++)
        {
            if (crc & 0x80)
            {
                crc = (uint8_t)((crc << 1) ^ 0x07);
            }
            else
            {
                crc <<= 1;
            }
        }
    }

    return crc;
}

static int verify_crc(uint8_t rx_buf[], size_t length)
{
    if (length < 3)
    {
        return 0;
    }

    // Special cases documented by TI for SPI responses
    if (rx_buf[0] == 0xFF && rx_buf[1] == 0xFF &&
        (rx_buf[2] == 0xFF || rx_buf[2] == 0x00))
    {
        return 1;
    }

    return rx_buf[2] == calculate_crc(rx_buf, 2);
}

static int is_internal_osc_asleep(uint8_t rx_buf[])
{
    return rx_buf[0] == 0xFF && rx_buf[1] == 0xFF && rx_buf[2] == 0xFF;
}

static int is_outgoing_buffer_not_updated(uint8_t rx_buf[])
{
    return rx_buf[0] == 0xFF && rx_buf[1] == 0xFF && rx_buf[2] == 0x00;
}

static int spi_write_read(uint8_t tx_buf[], uint8_t rx_buf[], size_t length)
{
    gpio_put(PIN_CS, 0);
    spi_write_read_blocking(SPI_PORT, tx_buf, rx_buf, length);
    gpio_put(PIN_CS, 1);
    print_hex(rx_buf, length);
    return verify_crc(rx_buf, length);
}

static void spi_write(uint8_t tx_buf[], size_t length)
{
    gpio_put(PIN_CS, 0);
    spi_write_blocking(SPI_PORT, tx_buf, length);
    gpio_put(PIN_CS, 1);
}

static int spi_read(uint8_t rx_buf[], size_t length)
{
    gpio_put(PIN_CS, 0);
    spi_read_blocking(SPI_PORT, 0x00, rx_buf, length);
    gpio_put(PIN_CS, 1);
    print_hex(rx_buf, length);
    return verify_crc(rx_buf, length);
}

static uint8_t calculate_dm_checksum(uint16_t addr, const uint8_t *data, size_t len)
{
    uint16_t sum = (uint16_t)(addr & 0xFFU) + (uint16_t)((addr >> 8) & 0xFFU);

    for (size_t i = 0; i < len; i++)
    {
        sum = (uint16_t)(sum + data[i]);
    }

    return (uint8_t)(~sum);
}

// Write two consecutive direct-command registers as two separate SPI transactions.
// The BQ769x2 SPI interface only supports single-byte register writes per
// transaction (unlike I2C which supports block writes).
static int fw_bq76972_write_direct_pair(uint8_t command_lsb,
                                     uint8_t value_lsb,
                                     uint8_t command_msb,
                                     uint8_t value_msb)
{
    if ((uint8_t)(command_lsb + 1U) != command_msb)
    {
        return FW_BQ76972_SPI_UNKNOWN_ERR;
    }

    // First register write: command_lsb = value_lsb
    int result = fw_bq76972_write_direct(command_lsb, value_lsb);
    if (result != FW_BQ76972_SUCCESS)
    {
        return result;
    }

    // Second register write: command_msb = value_msb
    return fw_bq76972_write_direct(command_msb, value_msb);
}

// ============================================================================
// Public API Functions
// ============================================================================

void fw_bq76972_init(void)
{
    spi_init(SPI_PORT, SPI_BAUDRATE);
    spi_set_format(SPI_PORT, BITS_PER_TRANSFER, SPI_CPOL, SPI_CPHA, SPI_ORDER);

    gpio_set_function(PIN_RX, GPIO_FUNC_SPI);
    gpio_pull_up(PIN_RX);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_TX, GPIO_FUNC_SPI);

    gpio_set_function(PIN_CS, GPIO_FUNC_SIO);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
}

int fw_bq76972_read_direct(uint8_t command, uint8_t *data)
{
    if (data == NULL)
    {
        return FW_BQ76972_SPI_UNKNOWN_ERR;
    }

    size_t tries = 0;
    int err_code = FW_BQ76972_SPI_UNKNOWN_ERR;
    uint8_t tx_buf[3] = {command, 0x00, 0x00};
    uint8_t rx_buf[3] = {0};

    tx_buf[2] = calculate_crc(tx_buf, 2);

    while (tries < SPI_MAX_RETRIES)
    {
        spi_write(tx_buf, 3);
        sleep_us(SPI_TRANSACTION_DELAY_US);

        if (!spi_read(rx_buf, 3))
        {
            err_code = FW_BQ76972_SPI_CRC_MISMATCH_ERR;
            tries++;
            sleep_us(SPI_RETRY_BACKOFF_US);
            continue;
        }

        if (is_outgoing_buffer_not_updated(rx_buf))
        {
            err_code = FW_BQ76972_SPI_READ_TOO_FAST_ERR;
            tries++;
            sleep_us(SPI_RETRY_BACKOFF_US);
            continue;
        }

        if (is_internal_osc_asleep(rx_buf))
        {
            err_code = FW_BQ76972_SPI_INTERNAL_OSC_ASLEEP_ERR;
            tries++;
            fw_bq76972_wakeup();
            sleep_ms(WAKEUP_DELAY_MS);
            continue;
        }

        if (rx_buf[0] != command)
        {
            err_code = FW_BQ76972_SPI_UNEXPECTED_RESPONSE_ERR;
            tries++;
            sleep_us(SPI_RETRY_BACKOFF_US);
            continue;
        }

        *data = rx_buf[1];
        return FW_BQ76972_SUCCESS;
    }

    return err_code;
}

int fw_bq76972_write_direct(uint8_t command, uint8_t data)
{
    // BQ769x2 SPI protocol: bit 7 of the first byte indicates R/W.
    // Bit 7 = 0 for read, bit 7 = 1 for write.
    uint8_t tx_buf[3] = {(uint8_t)(command | 0x80U), data, 0x00};
    tx_buf[2] = calculate_crc(tx_buf, 2);
    spi_write(tx_buf, 3);
    sleep_us(SPI_TRANSACTION_DELAY_US);
    return FW_BQ76972_SUCCESS;
}

int fw_bq76972_send_subcommand(uint16_t subcommand)
{
    return fw_bq76972_write_direct_pair(SUBCOMMAND_LOWER,
                                     (uint8_t)(subcommand & 0xFFU),
                                     SUBCOMMAND_UPPER,
                                     (uint8_t)((subcommand >> 8) & 0xFFU));
}

int fw_bq76972_enter_config_update(void)
{
    int result = fw_bq76972_send_subcommand(0x0090);
    if (result == FW_BQ76972_SUCCESS)
    {
        sleep_ms(2);
    }
    return result;
}

int fw_bq76972_exit_config_update(void)
{
    int result = fw_bq76972_send_subcommand(0x0092);
    if (result == FW_BQ76972_SUCCESS)
    {
        sleep_ms(100);
    }
    return result;
}

int fw_bq76972_write_data_memory(uint16_t addr, const uint8_t *data, size_t len,
                              bool use_config_update)
{
    if (data == NULL || len == 0 || len > DATA_MEMORY_MAX_BYTES)
    {
        return FW_BQ76972_SPI_UNKNOWN_ERR;
    }

    int result = FW_BQ76972_SUCCESS;

    if (use_config_update)
    {
        result = fw_bq76972_enter_config_update();
        if (result != FW_BQ76972_SUCCESS)
        {
            return result;
        }
    }

    result = fw_bq76972_write_direct_pair(SUBCOMMAND_LOWER,
                                       (uint8_t)(addr & 0xFFU),
                                       SUBCOMMAND_UPPER,
                                       (uint8_t)((addr >> 8) & 0xFFU));

    for (size_t i = 0; i < len && result == FW_BQ76972_SUCCESS; i++)
    {
        result = fw_bq76972_write_direct((uint8_t)(RESPONSE_BUFFER_START + i), data[i]);
    }

    if (result == FW_BQ76972_SUCCESS)
    {
        uint8_t checksum = calculate_dm_checksum(addr, data, len);
        uint8_t total_length = (uint8_t)(len + 4U);

        result = fw_bq76972_write_direct_pair(RESPONSE_BUFFER_CHKSUM,
                                           checksum,
                                           RESPONSE_BUFFER_LENGTH,
                                           total_length);
    }

    if (use_config_update)
    {
        int exit_result = fw_bq76972_exit_config_update();
        if (result == FW_BQ76972_SUCCESS)
        {
            result = exit_result;
        }
    }

    if (result == FW_BQ76972_SUCCESS)
    {
        sleep_ms(CONFIG_SETTLE_DELAY_MS);
    }

    return result;
}

int fw_bq76972_write_data_memory_u8(uint16_t addr, uint8_t value, bool use_config_update)
{
    return fw_bq76972_write_data_memory(addr, &value, 1, use_config_update);
}

int fw_bq76972_write_data_memory_u16(uint16_t addr, uint16_t value, bool use_config_update)
{
    uint8_t bytes[2] = {
        (uint8_t)(value & 0xFFU),
        (uint8_t)((value >> 8) & 0xFFU),
    };

    return fw_bq76972_write_data_memory(addr, bytes, 2, use_config_update);
}

int fw_bq76972_read_data_memory(uint16_t addr, uint8_t *data, size_t len)
{
    if (data == NULL || len == 0 || len > DATA_MEMORY_MAX_BYTES)
    {
        return FW_BQ76972_SPI_UNKNOWN_ERR;
    }

    int result = fw_bq76972_write_direct_pair(SUBCOMMAND_LOWER,
                                           (uint8_t)(addr & 0xFFU),
                                           SUBCOMMAND_UPPER,
                                           (uint8_t)((addr >> 8) & 0xFFU));
    if (result != FW_BQ76972_SUCCESS)
    {
        return result;
    }

    // Poll until the response buffer is fully populated:
    //   - 0x3E echoes back the lower address byte
    //   - 0x61 (response length) is non-zero
    bool ready = false;
    for (int attempt = 0; attempt < 3 && !ready; attempt++)
    {
        if (attempt > 0)
        {
            fw_bq76972_wakeup();
            result = fw_bq76972_write_direct_pair(SUBCOMMAND_LOWER,
                                               (uint8_t)(addr & 0xFFU),
                                               SUBCOMMAND_UPPER,
                                               (uint8_t)((addr >> 8) & 0xFFU));
            if (result != FW_BQ76972_SUCCESS)
            {
                return result;
            }
        }

        for (int poll = 0; poll < 50; poll++)
        {
            uint8_t subcmd_lo = 0xFF;
            uint8_t resp_len = 0;

            result = fw_bq76972_read_direct(SUBCOMMAND_LOWER, &subcmd_lo);
            if (result == FW_BQ76972_SUCCESS)
            {
                result = fw_bq76972_read_direct(RESPONSE_BUFFER_LENGTH, &resp_len);
            }

            if (result == FW_BQ76972_SUCCESS &&
                subcmd_lo == (uint8_t)(addr & 0xFFU) && resp_len > 0)
            {
                ready = true;
                break;
            }
            sleep_ms(2);
        }
    }

    if (!ready)
    {
        printf("[DM_READ 0x%04X] Response buffer never ready\n", addr);
        return FW_BQ76972_SPI_READ_TOO_FAST_ERR;
    }

    // Read length to determine how many buffer bytes the device populated.
    // Length = subcmd(2) + chk/len(2) + buffer_data_bytes.
    uint8_t reported_length = 0;
    result = fw_bq76972_read_direct(RESPONSE_BUFFER_LENGTH, &reported_length);
    if (result != FW_BQ76972_SUCCESS)
    {
        return result;
    }

    size_t buf_data_len = (reported_length > 4) ? (reported_length - 4) : 0;
    if (buf_data_len == 0 || buf_data_len > DATA_MEMORY_MAX_BYTES)
    {
        printf("[DM_READ 0x%04X] Invalid response length: %d\n",
               addr, reported_length);
        return FW_BQ76972_SPI_UNEXPECTED_RESPONSE_ERR;
    }

    // Read the full data block from the transfer buffer (0x40..0x5F).
    uint8_t block[DATA_MEMORY_MAX_BYTES] = {0};
    for (size_t i = 0; i < buf_data_len; i++)
    {
        result = fw_bq76972_read_direct(
            (uint8_t)(RESPONSE_BUFFER_START + i), &block[i]);
        if (result != FW_BQ76972_SUCCESS)
        {
            printf("[DM_READ 0x%04X] Failed reading 0x%02X (err=%d)\n",
                   addr, (unsigned)(RESPONSE_BUFFER_START + i), result);
            return result;
        }
    }

    // Read the reported checksum.
    uint8_t reported_checksum = 0;
    result = fw_bq76972_read_direct(RESPONSE_BUFFER_CHKSUM, &reported_checksum);
    if (result != FW_BQ76972_SUCCESS)
    {
        return result;
    }

    // Verify checksum: ~(0x3E_lo + 0x3F_hi + all buffer data bytes).
    // The checksum covers the subcommand address bytes + full buffer block.
    uint8_t computed_checksum = calculate_dm_checksum(addr, block, buf_data_len);

    printf("[DM_READ 0x%04X] block_len=%zu chk: reported=0x%02X computed=0x%02X\n",
           addr, buf_data_len, reported_checksum, computed_checksum);

    if (reported_checksum != computed_checksum)
    {
        printf("[DM_READ 0x%04X] Checksum mismatch\n", addr);
        return FW_BQ76972_SPI_CRC_MISMATCH_ERR;
    }

    // The device always returns a full 32-byte block aligned to the base
    // address written to 0x3E/0x3F. For data memory, the requested byte(s)
    // start at offset 0 within the block (the address we wrote is the base).
    if (len > buf_data_len)
    {
        printf("[DM_READ 0x%04X] Requested %zu bytes but block has %zu\n",
               addr, len, buf_data_len);
        return FW_BQ76972_SPI_UNEXPECTED_RESPONSE_ERR;
    }

    for (size_t i = 0; i < len; i++)
    {
        data[i] = block[i];
    }

    return FW_BQ76972_SUCCESS;
}

int fw_bq76972_read_data_memory_u8(uint16_t addr, uint8_t *value)
{
    return fw_bq76972_read_data_memory(addr, value, 1);
}

int fw_bq76972_read_data_memory_u16(uint16_t addr, uint16_t *value)
{
    if (value == NULL)
    {
        return FW_BQ76972_SPI_UNKNOWN_ERR;
    }

    uint8_t bytes[2] = {0};
    int result = fw_bq76972_read_data_memory(addr, bytes, 2);
    if (result == FW_BQ76972_SUCCESS)
    {
        *value = (uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8);
    }
    return result;
}

int fw_bq76972_read_u16(uint8_t lsb_command, uint16_t *value)
{
    if (value == NULL)
    {
        return FW_BQ76972_SPI_UNKNOWN_ERR;
    }

    uint8_t lsb = 0;
    uint8_t msb = 0;
    int last_error = FW_BQ76972_SPI_UNKNOWN_ERR;

    for (int i = 0; i < SPI_MAX_RETRIES; i++)
    {
        int result = fw_bq76972_read_direct(lsb_command, &lsb);
        if (result != FW_BQ76972_SUCCESS)
        {
            last_error = result;
            continue;
        }

        result = fw_bq76972_read_direct((uint8_t)(lsb_command + 1U), &msb);
        if (result != FW_BQ76972_SUCCESS)
        {
            last_error = result;
            continue;
        }

        *value = (uint16_t)lsb | ((uint16_t)msb << 8);
        return FW_BQ76972_SUCCESS;
    }

    return last_error;
}

int fw_bq76972_read_cell_voltage(fw_cell_voltage_t cell, uint16_t *voltage_mv)
{
    return fw_bq76972_read_u16((uint8_t)cell, voltage_mv);
}

int fw_bq76972_read_all_cell_voltages(uint16_t *voltages, size_t num_cells)
{
    if (voltages == NULL || num_cells == 0)
    {
        return 0;
    }

    for (size_t i = 0; i < num_cells; i++)
    {
        fw_cell_voltage_t cell = (fw_cell_voltage_t)(FW_CELL_1 + (i * 2));
        if (fw_bq76972_read_cell_voltage(cell, &voltages[i]) != FW_BQ76972_SUCCESS)
        {
            return 0;
        }
    }

    return 1;
}

int fw_bq76972_read_temperature(fw_thermistor_t sensor, int16_t *temperature_dK)
{
    if (temperature_dK == NULL)
    {
        return FW_BQ76972_SPI_UNKNOWN_ERR;
    }

    uint16_t raw_value = 0;
    int result = fw_bq76972_read_u16((uint8_t)sensor, &raw_value);
    if (result == FW_BQ76972_SUCCESS)
    {
        *temperature_dK = (int16_t)raw_value;
    }

    return result;
}

int fw_bq76972_read_all_temperatures(const fw_thermistor_t *sensors,
                                  int16_t *temperatures_dK,
                                  size_t num_sensors)
{
    if (sensors == NULL || temperatures_dK == NULL || num_sensors == 0)
    {
        return FW_BQ76972_SPI_UNKNOWN_ERR;
    }

    for (size_t i = 0; i < num_sensors; i++)
    {
        int result = fw_bq76972_read_temperature(sensors[i], &temperatures_dK[i]);
        if (result != FW_BQ76972_SUCCESS)
        {
            return result;
        }
    }

    return FW_BQ76972_SUCCESS;
}

float fw_bq76972_temperature_dK_to_c(int16_t temperature_dK)
{
    return ((float)temperature_dK / 10.0f) - 273.15f;
}

int fw_bq76972_configure_thermistors(uint8_t ts1_config,
                                  uint8_t ts2_config,
                                  uint8_t ts3_config,
                                  uint8_t dfetoff_config,
                                  uint8_t dchg_config,
                                  uint8_t ddsg_config)
{
    int result = fw_bq76972_enter_config_update();
    if (result != FW_BQ76972_SUCCESS)
    {
        return result;
    }

    result = fw_bq76972_write_data_memory_u8(DFETOFF_CONFIG_ADDR, dfetoff_config, false);
    if (result == FW_BQ76972_SUCCESS)
    {
        result = fw_bq76972_write_data_memory_u8(TS1_CONFIG_ADDR, ts1_config, false);
    }
    if (result == FW_BQ76972_SUCCESS)
    {
        result = fw_bq76972_write_data_memory_u8(TS2_CONFIG_ADDR, ts2_config, false);
    }
    if (result == FW_BQ76972_SUCCESS)
    {
        result = fw_bq76972_write_data_memory_u8(TS3_CONFIG_ADDR, ts3_config, false);
    }
    if (result == FW_BQ76972_SUCCESS)
    {
        result = fw_bq76972_write_data_memory_u8(DCHG_CONFIG_ADDR, dchg_config, false);
    }
    if (result == FW_BQ76972_SUCCESS)
    {
        result = fw_bq76972_write_data_memory_u8(DDSG_CONFIG_ADDR, ddsg_config, false);
    }

    {
        int exit_result = fw_bq76972_exit_config_update();
        if (result == FW_BQ76972_SUCCESS)
        {
            result = exit_result;
        }
    }

    if (result == FW_BQ76972_SUCCESS)
    {
        sleep_ms(CONFIG_SETTLE_DELAY_MS);
    }

    return result;
}

int fw_bq76972_configure_thermistors_default(void)
{
    return fw_bq76972_configure_thermistors(THERMISTOR_TS_DEFAULT_CONFIG,
                                         THERMISTOR_TS_DEFAULT_CONFIG,
                                         THERMISTOR_TS_DEFAULT_CONFIG,
                                         THERMISTOR_MF_DEFAULT_CONFIG,
                                         THERMISTOR_MF_DEFAULT_CONFIG,
                                         THERMISTOR_MF_DEFAULT_CONFIG);
}

int fw_bq76972_read_thermistor_pin_configs(uint8_t *ts1_config,
                                        uint8_t *ts2_config,
                                        uint8_t *ts3_config,
                                        uint8_t *dfetoff_config,
                                        uint8_t *dchg_config,
                                        uint8_t *ddsg_config)
{
    if (ts1_config == NULL || ts2_config == NULL || ts3_config == NULL ||
        dfetoff_config == NULL || dchg_config == NULL || ddsg_config == NULL)
    {
        return FW_BQ76972_SPI_UNKNOWN_ERR;
    }

    int result = fw_bq76972_read_data_memory_u8(DFETOFF_CONFIG_ADDR, dfetoff_config);
    if (result == FW_BQ76972_SUCCESS)
    {
        result = fw_bq76972_read_data_memory_u8(TS1_CONFIG_ADDR, ts1_config);
    }
    if (result == FW_BQ76972_SUCCESS)
    {
        result = fw_bq76972_read_data_memory_u8(TS2_CONFIG_ADDR, ts2_config);
    }
    if (result == FW_BQ76972_SUCCESS)
    {
        result = fw_bq76972_read_data_memory_u8(TS3_CONFIG_ADDR, ts3_config);
    }
    if (result == FW_BQ76972_SUCCESS)
    {
        result = fw_bq76972_read_data_memory_u8(DCHG_CONFIG_ADDR, dchg_config);
    }
    if (result == FW_BQ76972_SUCCESS)
    {
        result = fw_bq76972_read_data_memory_u8(DDSG_CONFIG_ADDR, ddsg_config);
    }

    return result;
}

int fw_bq76972_verify_thermistor_config(void)
{
    uint8_t ts1, ts2, ts3, dfetoff, dchg, ddsg;

    int result = fw_bq76972_read_thermistor_pin_configs(&ts1, &ts2, &ts3,
                                                     &dfetoff, &dchg, &ddsg);
    if (result != FW_BQ76972_SUCCESS)
    {
        printf("[BQ76972] Failed to read thermistor configs (err=%d)\n", result);
        return result;
    }

    printf("[BQ76972] Thermistor config readback:\n");
    printf("  TS1=0x%02X  TS2=0x%02X  TS3=0x%02X\n", ts1, ts2, ts3);
    printf("  DFETOFF=0x%02X  DCHG=0x%02X  DDSG=0x%02X\n", dfetoff, dchg, ddsg);

    int ok = 1;
    if (ts1 == 0x00)
    {
        printf("[BQ76972] WARNING: TS1 config is 0x00 (not enabled)\n");
        ok = 0;
    }
    if (ts2 == 0x00)
    {
        printf("[BQ76972] WARNING: TS2 config is 0x00 (not enabled)\n");
        ok = 0;
    }
    if (ts3 == 0x00)
    {
        printf("[BQ76972] WARNING: TS3 config is 0x00 (not enabled)\n");
        ok = 0;
    }
    if (dfetoff == 0x00)
    {
        printf("[BQ76972] WARNING: DFETOFF config is 0x00 (not enabled)\n");
        ok = 0;
    }
    if (dchg == 0x00)
    {
        printf("[BQ76972] WARNING: DCHG config is 0x00 (not enabled)\n");
        ok = 0;
    }
    if (ddsg == 0x00)
    {
        printf("[BQ76972] WARNING: DDSG config is 0x00 (not enabled)\n");
        ok = 0;
    }

    if (!ok)
    {
        printf("[BQ76972] One or more thermistor pins failed to configure!\n");
    }

    return ok ? FW_BQ76972_SUCCESS : FW_BQ76972_SPI_UNEXPECTED_RESPONSE_ERR;
}

int fw_bq76972_configure_user_amps(fw_user_amps_t setting)
{
    uint8_t da_config = 0;
    int result = fw_bq76972_read_data_memory_u8(DA_CONFIG_ADDR, &da_config);
    if (result != FW_BQ76972_SUCCESS)
    {
        return result;
    }

    // USER_AMPS occupies bits [1:0]
    da_config = (uint8_t)((da_config & 0xFC) | ((uint8_t)setting & 0x03));

    result = fw_bq76972_write_data_memory_u8(DA_CONFIG_ADDR, da_config, true);
    if (result == FW_BQ76972_SUCCESS)
    {
        cached_user_amps = setting;
    }
    return result;
}

int fw_bq76972_read_user_amps(fw_user_amps_t *setting)
{
    if (setting == NULL)
    {
        return FW_BQ76972_SPI_UNKNOWN_ERR;
    }

    uint8_t da_config = 0;
    int result = fw_bq76972_read_data_memory_u8(DA_CONFIG_ADDR, &da_config);
    if (result == FW_BQ76972_SUCCESS)
    {
        *setting = (fw_user_amps_t)(da_config & 0x03);
        cached_user_amps = *setting;
    }
    return result;
}

int fw_bq76972_read_current_raw(int16_t *raw)
{
    if (raw == NULL)
    {
        return FW_BQ76972_SPI_UNKNOWN_ERR;
    }

    uint16_t val = 0;
    int result = fw_bq76972_read_u16(0x3A, &val);
    if (result == FW_BQ76972_SUCCESS)
    {
        *raw = (int16_t)val;
    }
    return result;
}

int fw_bq76972_read_current_mA(int32_t *current_mA)
{
    if (current_mA == NULL)
    {
        return FW_BQ76972_SPI_UNKNOWN_ERR;
    }

    int16_t raw = 0;
    int result = fw_bq76972_read_current_raw(&raw);
    if (result != FW_BQ76972_SUCCESS)
    {
        return result;
    }

    static const float scale[] = {0.1f, 1.0f, 10.0f, 100.0f};
    *current_mA = (int32_t)((float)raw * scale[cached_user_amps & 0x03]);
    return FW_BQ76972_SUCCESS;
}

int fw_bq76972_read_all_thermistor_values(int16_t *temperatures_dK,
                                       size_t num_temperatures)
{
    static const fw_thermistor_t thermistors[FW_BQ76972_THERMISTOR_COUNT] = {
        FW_TS1_THERMISTOR,
        FW_TS2_THERMISTOR,
        FW_TS3_THERMISTOR,
        FW_DFETOFF_THERMISTOR,
        FW_DCHG_THERMISTOR,
        FW_DDSG_THERMISTOR,
    };

    if (temperatures_dK == NULL ||
        num_temperatures < (size_t)FW_BQ76972_THERMISTOR_COUNT)
    {
        return FW_BQ76972_SPI_UNKNOWN_ERR;
    }

    fw_bq76972_wakeup();
    sleep_ms(2);

    return fw_bq76972_read_all_temperatures(thermistors,
                                         temperatures_dK,
                                         FW_BQ76972_THERMISTOR_COUNT);
}

void fw_bq76972_wakeup(void)
{
    uint8_t tx_buf[3] = {0x00, 0x00, 0x00};
    tx_buf[2] = calculate_crc(tx_buf, 2);

    spi_write(tx_buf, 3);
    sleep_ms(WAKEUP_DELAY_MS);
    (void)spi_read(tx_buf, 3);
}