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
    CELL_1 = 0x14,
    CELL_2 = 0x16,
    CELL_3 = 0x18,
    CELL_4 = 0x1A,
    CELL_5 = 0x1C,
    CELL_6 = 0x1E,
    CELL_7 = 0x20,
    CELL_8 = 0x22,
    CELL_9 = 0x24,
    CELL_10 = 0x26,
    CELL_11 = 0x28,
    CELL_12 = 0x2A,
    CELL_13 = 0x2C,
    CELL_14 = 0x2E,
    CELL_15 = 0x30,
    CELL_16 = 0x32,
    LAST_CELL = CELL_16,
    STACK_VOLTAGE = 0x34
} cell_voltage_t;

// ============================================================================
// Temperature Direct Command Addresses
// ============================================================================
typedef enum
{
    INT_THERMISTOR = 0x68,
    CFETOFF_THERMISTOR = 0x6A,
    DFETOFF_THERMISTOR = 0x6C,
    ALERT_THERMISTOR = 0x6E,
    TS1_THERMISTOR = 0x70,
    TS2_THERMISTOR = 0x72,
    TS3_THERMISTOR = 0x74,
    HDQ_THERMISTOR = 0x76,
    DCHG_THERMISTOR = 0x78,
    DDSG_THERMISTOR = 0x7A,
} thermistor_t;

#define BQ76972_THERMISTOR_COUNT 6

// ============================================================================
// Error Codes
// ============================================================================
#define SUCCESS 1
#define SPI_ERR(x) (100 + (x))
#define SPI_UNKNOWN_ERR SPI_ERR(0)
#define SPI_READ_TOO_FAST_ERR SPI_ERR(1)
#define SPI_INTERNAL_OSC_ASLEEP_ERR SPI_ERR(2)
#define SPI_CRC_MISMATCH_ERR SPI_ERR(3)
#define SPI_UNEXPECTED_RESPONSE_ERR SPI_ERR(4)

// ============================================================================
// Initialization
// ============================================================================
void bq76972_init(void);

// ============================================================================
// Direct Command Functions (Single Byte Read/Write)
// ============================================================================
int bq76972_read_direct(uint8_t command, uint8_t *data);
int bq76972_write_direct(uint8_t command, uint8_t data);
int bq76972_send_subcommand(uint16_t subcommand);
int bq76972_enter_config_update(void);
int bq76972_exit_config_update(void);

// ============================================================================
// Data Memory Functions
// ============================================================================
int bq76972_write_data_memory(uint16_t addr, const uint8_t *data, size_t len,
                              bool use_config_update);
int bq76972_write_data_memory_u8(uint16_t addr, uint8_t value,
                                 bool use_config_update);
int bq76972_write_data_memory_u16(uint16_t addr, uint16_t value,
                                  bool use_config_update);

int bq76972_read_data_memory(uint16_t addr, uint8_t *data, size_t len);
int bq76972_read_data_memory_u8(uint16_t addr, uint8_t *value);
int bq76972_read_data_memory_u16(uint16_t addr, uint16_t *value);

// ============================================================================
// 16-bit Value Functions (LSB/MSB Read)
// ============================================================================
int bq76972_read_u16(uint8_t lsb_command, uint16_t *value);

// ============================================================================
// Voltage Reading Functions
// ============================================================================
int bq76972_read_cell_voltage(cell_voltage_t cell, uint16_t *voltage_mv);
int bq76972_read_all_cell_voltages(uint16_t *voltages, size_t num_cells);

// ============================================================================
// Temperature Reading Functions
// NOTE: the device reports temperature direct commands in 0.1 K units.
// ============================================================================
int bq76972_read_temperature(thermistor_t sensor, int16_t *temperature_dK);
int bq76972_read_all_temperatures(const thermistor_t *sensors,
                                  int16_t *temperatures_dK,
                                  size_t num_sensors);
float bq76972_temperature_dK_to_c(int16_t temperature_dK);

// ============================================================================
// Thermistor Pin Configuration
// These configure TS1 / TS2 / TS3 / DFETOFF / DCHG / DDSG as thermistor inputs.
// ============================================================================
int bq76972_configure_thermistors(uint8_t ts1_config,
                                  uint8_t ts2_config,
                                  uint8_t ts3_config,
                                  uint8_t dfetoff_config,
                                  uint8_t dchg_config,
                                  uint8_t ddsg_config);

int bq76972_configure_thermistors_default(void);

int bq76972_read_thermistor_pin_configs(uint8_t *ts1_config,
                                        uint8_t *ts2_config,
                                        uint8_t *ts3_config,
                                        uint8_t *dfetoff_config,
                                        uint8_t *dchg_config,
                                        uint8_t *ddsg_config);

int bq76972_read_all_thermistor_values(int16_t *temperatures_dK,
                                       size_t num_temperatures);

// ============================================================================
// Current Reading Functions
// CC2 Current (0x3A): signed 16-bit, units determined by USER_AMPS setting
// in DA Configuration (0x9303).
// ============================================================================

// USER_AMPS setting in DA Configuration register
typedef enum
{
    USER_AMPS_0_1_MA = 0, // 0.1 mA per count
    USER_AMPS_1_MA = 1,   // 1 mA per count (default)
    USER_AMPS_10_MA = 2,  // 10 mA per count
    USER_AMPS_100_MA = 3, // 100 mA per count
} user_amps_t;

int bq76972_configure_user_amps(user_amps_t setting);
int bq76972_read_user_amps(user_amps_t *setting);

// Reads the raw CC2 current value. Units depend on the USER_AMPS setting.
int bq76972_read_current_raw(int16_t *raw);

// Reads CC2 current and converts to milliamps using the current USER_AMPS
// setting. Call bq76972_configure_user_amps() first, or the default (1 mA)
// is assumed.
int bq76972_read_current_mA(int32_t *current_mA);

// ============================================================================
// Configuration Verification
// ============================================================================
int bq76972_verify_thermistor_config(void);

// ============================================================================
// Power Management
// ============================================================================
void bq76972_wakeup(void);

#endif // FW_BQ76972_H