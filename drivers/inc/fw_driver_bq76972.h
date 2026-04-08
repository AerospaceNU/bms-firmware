#ifndef FW_BQ76972_H
#define FW_BQ76972_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// ============================================================================
// Cell Voltage Addresses
// ============================================================================
typedef enum
{
    FW_CELL_1 = 0x14,
    FW_CELL_2 = 0x16,
    FW_CELL_3 = 0x18,
    FW_CELL_4 = 0x1A,
    FW_CELL_5 = 0x1C,
    FW_CELL_6 = 0x1E,
    FW_CELL_7 = 0x20,
    FW_CELL_8 = 0x22,
    FW_CELL_9 = 0x24,
    FW_CELL_10 = 0x26,
    FW_CELL_11 = 0x28,
    FW_CELL_12 = 0x2A,
    FW_CELL_13 = 0x2C,
    FW_CELL_14 = 0x2E,
    FW_CELL_15 = 0x30,
    FW_CELL_16 = 0x32,
    FW_LAST_CELL = FW_CELL_16,
    FW_STACK_VOLTAGE = 0x34
} fw_cell_voltage_t;

// ============================================================================
// Temperature Direct Command Addresses
// ============================================================================
typedef enum
{
    FW_INT_THERMISTOR = 0x68,
    FW_CFETOFF_THERMISTOR = 0x6A,
    FW_DFETOFF_THERMISTOR = 0x6C,
    FW_ALERT_THERMISTOR = 0x6E,
    FW_TS1_THERMISTOR = 0x70,
    FW_TS2_THERMISTOR = 0x72,
    FW_TS3_THERMISTOR = 0x74,
    FW_HDQ_THERMISTOR = 0x76,
    FW_DCHG_THERMISTOR = 0x78,
    FW_DDSG_THERMISTOR = 0x7A,
} fw_thermistor_t;

#define FW_BQ76972_THERMISTOR_COUNT 6

// ============================================================================
// Error Codes
// ============================================================================
#define FW_BQ76972_SUCCESS 1
#define FW_BQ76972_SPI_ERR(x) (100 + (x))
#define FW_BQ76972_SPI_UNKNOWN_ERR FW_BQ76972_SPI_ERR(0)
#define FW_BQ76972_SPI_READ_TOO_FAST_ERR FW_BQ76972_SPI_ERR(1)
#define FW_BQ76972_SPI_INTERNAL_OSC_ASLEEP_ERR FW_BQ76972_SPI_ERR(2)
#define FW_BQ76972_SPI_CRC_MISMATCH_ERR FW_BQ76972_SPI_ERR(3)
#define FW_BQ76972_SPI_UNEXPECTED_RESPONSE_ERR FW_BQ76972_SPI_ERR(4)

// ============================================================================
// Initialization
// ============================================================================
void fw_bq76972_init(void);

// ============================================================================
// Direct Command Functions (Single Byte Read/Write)
// ============================================================================
int fw_bq76972_read_direct(uint8_t command, uint8_t *data);
int fw_bq76972_write_direct(uint8_t command, uint8_t data);
int fw_bq76972_send_subcommand(uint16_t subcommand);
int fw_bq76972_enter_config_update(void);
int fw_bq76972_exit_config_update(void);

// ============================================================================
// Data Memory Functions
// ============================================================================
int fw_bq76972_write_data_memory(uint16_t addr, const uint8_t *data, size_t len,
                              bool use_config_update);
int fw_bq76972_write_data_memory_u8(uint16_t addr, uint8_t value,
                                 bool use_config_update);
int fw_bq76972_write_data_memory_u16(uint16_t addr, uint16_t value,
                                  bool use_config_update);

int fw_bq76972_read_data_memory(uint16_t addr, uint8_t *data, size_t len);
int fw_bq76972_read_data_memory_u8(uint16_t addr, uint8_t *value);
int fw_bq76972_read_data_memory_u16(uint16_t addr, uint16_t *value);

// ============================================================================
// 16-bit Value Functions (LSB/MSB Read)
// ============================================================================
int fw_bq76972_read_u16(uint8_t lsb_command, uint16_t *value);

// ============================================================================
// Voltage Reading Functions
// ============================================================================
int fw_bq76972_read_cell_voltage(fw_cell_voltage_t cell, uint16_t *voltage_mv);
int fw_bq76972_read_all_cell_voltages(uint16_t *voltages, size_t num_cells);

// ============================================================================
// Temperature Reading Functions
// NOTE: the device reports temperature direct commands in 0.1 K units.
// ============================================================================
int fw_bq76972_read_temperature(fw_thermistor_t sensor, int16_t *temperature_dK);
int fw_bq76972_read_all_temperatures(const fw_thermistor_t *sensors,
                                  int16_t *temperatures_dK,
                                  size_t num_sensors);
float fw_bq76972_temperature_dK_to_c(int16_t temperature_dK);

// ============================================================================
// Thermistor Pin Configuration
// These configure TS1 / TS2 / TS3 / DFETOFF / DCHG / DDSG as thermistor inputs.
// ============================================================================
int fw_bq76972_configure_thermistors(uint8_t ts1_config,
                                  uint8_t ts2_config,
                                  uint8_t ts3_config,
                                  uint8_t dfetoff_config,
                                  uint8_t dchg_config,
                                  uint8_t ddsg_config);

int fw_bq76972_configure_thermistors_default(void);

int fw_bq76972_read_thermistor_pin_configs(uint8_t *ts1_config,
                                        uint8_t *ts2_config,
                                        uint8_t *ts3_config,
                                        uint8_t *dfetoff_config,
                                        uint8_t *dchg_config,
                                        uint8_t *ddsg_config);

int fw_bq76972_read_all_thermistor_values(int16_t *temperatures_dK,
                                       size_t num_temperatures);

// ============================================================================
// Current Reading Functions
// CC2 Current (0x3A): signed 16-bit, units determined by USER_AMPS setting
// in DA Configuration (0x9303).
// ============================================================================

// USER_AMPS setting in DA Configuration register
typedef enum
{
    FW_USER_AMPS_0_1_MA = 0, // 0.1 mA per count
    FW_USER_AMPS_1_MA = 1,   // 1 mA per count (default)
    FW_USER_AMPS_10_MA = 2,  // 10 mA per count
    FW_USER_AMPS_100_MA = 3, // 100 mA per count
} fw_user_amps_t;

int fw_bq76972_configure_user_amps(fw_user_amps_t setting);
int fw_bq76972_read_user_amps(fw_user_amps_t *setting);

// Reads the raw CC2 current value. Units depend on the USER_AMPS setting.
int fw_bq76972_read_current_raw(int16_t *raw);

// Reads CC2 current and converts to milliamps using the current USER_AMPS
// setting. Call fw_bq76972_configure_user_amps() first, or the default (1 mA)
// is assumed.
int fw_bq76972_read_current_mA(int32_t *current_mA);

// ============================================================================
// Configuration Verification
// ============================================================================
int fw_bq76972_verify_thermistor_config(void);

// ============================================================================
// Power Management
// ============================================================================
void fw_bq76972_wakeup(void);

#endif // FW_BQ76972_H