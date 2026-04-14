#ifndef FW_BQ76972_H
#define FW_BQ76972_H

/**
 * \file fw_driver_bq76972.h
 * \brief Driver interface for the BQ76972 16-cell battery monitor via SPI.
 *
 * Provides direct-command reads/writes, data-memory access, cell voltage and
 * temperature acquisition, current measurement, and thermistor pin configuration
 * for the Texas Instruments BQ76972 battery monitor IC.
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "fw_logic_error.h"

// ============================================================================
// Cell Voltage Addresses
// ============================================================================

/**
 * \brief SPI direct-command register addresses for individual cell voltages.
 *
 * Each address maps to a two-byte (LSB + MSB) voltage register on the device.
 * Reading returns the cell voltage with 1 mV per LSB resolution.
 */
typedef enum
{
    FW_CELL_1       = 0x14, /**< Cell 1 voltage register  */
    FW_CELL_2       = 0x16, /**< Cell 2 voltage register  */
    FW_CELL_3       = 0x18, /**< Cell 3 voltage register  */
    FW_CELL_4       = 0x1A, /**< Cell 4 voltage register  */
    FW_CELL_5       = 0x1C, /**< Cell 5 voltage register  */
    FW_CELL_6       = 0x1E, /**< Cell 6 voltage register  */
    FW_CELL_7       = 0x20, /**< Cell 7 voltage register  */
    FW_CELL_8       = 0x22, /**< Cell 8 voltage register  */
    FW_CELL_9       = 0x24, /**< Cell 9 voltage register  */
    FW_CELL_10      = 0x26, /**< Cell 10 voltage register */
    FW_CELL_11      = 0x28, /**< Cell 11 voltage register */
    FW_CELL_12      = 0x2A, /**< Cell 12 voltage register */
    FW_CELL_13      = 0x2C, /**< Cell 13 voltage register */
    FW_CELL_14      = 0x2E, /**< Cell 14 voltage register */
    FW_CELL_15      = 0x30, /**< Cell 15 voltage register */
    FW_CELL_16      = 0x32, /**< Cell 16 voltage register */
    FW_LAST_CELL    = FW_CELL_16, /**< Sentinel: last valid cell address */
    FW_STACK_VOLTAGE = 0x34, /**< Total stack voltage register */
} fw_cell_voltage_t;

// ============================================================================
// Temperature Direct Command Addresses
// ============================================================================

/**
 * \brief SPI direct-command register addresses for temperature sensors.
 *
 * Each address maps to a two-byte temperature register. Raw values are in
 * 0.1 K units (decikelvin). Use \ref fw_bq76972_temperature_dK_to_c to convert
 * to degrees Celsius.
 */
typedef enum
{
    FW_INT_THERMISTOR    = 0x68, /**< Internal die thermistor         */
    FW_CFETOFF_THERMISTOR = 0x6A, /**< CFETOFF pin thermistor          */
    FW_DFETOFF_THERMISTOR = 0x6C, /**< DFETOFF pin thermistor          */
    FW_ALERT_THERMISTOR  = 0x6E, /**< ALERT pin thermistor            */
    FW_TS1_THERMISTOR    = 0x70, /**< TS1 pin thermistor              */
    FW_TS2_THERMISTOR    = 0x72, /**< TS2 pin thermistor              */
    FW_TS3_THERMISTOR    = 0x74, /**< TS3 pin thermistor              */
    FW_HDQ_THERMISTOR    = 0x76, /**< HDQ pin thermistor              */
    FW_DCHG_THERMISTOR   = 0x78, /**< DCHG pin thermistor             */
    FW_DDSG_THERMISTOR   = 0x7A, /**< DDSG pin thermistor             */
} fw_thermistor_t;

/** \brief Number of external thermistor pins used by this firmware. */
#define FW_BQ76972_THERMISTOR_COUNT 6

// ============================================================================
// Initialization
// ============================================================================

/**
 * \brief Initialize the BQ76972 driver and underlying SPI peripheral.
 *
 * Must be called once before any other function in this driver.
 */
void fw_bq76972_init(void);

// ============================================================================
// Direct Command Functions (Single Byte Read/Write)
// ============================================================================

/**
 * \brief Read one byte from a direct-command register.
 *
 * \param[in]  command  Direct-command address (8-bit).
 * \param[out] data     Pointer to store the received byte.
 * \return \ref FW_BQ76972_SUCCESS on success, or a \ref FW_BQ76972_SPI_ERR code.
 */
int fw_bq76972_read_direct(uint8_t command, uint8_t *data);

/**
 * \brief Write one byte to a direct-command register.
 *
 * \param[in] command  Direct-command address (8-bit).
 * \param[in] data     Byte value to write.
 * \return \ref FW_BQ76972_SUCCESS on success, or a \ref FW_BQ76972_SPI_ERR code.
 */
int fw_bq76972_write_direct(uint8_t command, uint8_t data);

/**
 * \brief Issue a 16-bit subcommand to the device.
 *
 * Writes the subcommand bytes to the SUBCMD registers (0x3E / 0x3F) and
 * triggers execution. Does not read back a response.
 *
 * \param[in] subcommand  16-bit subcommand word.
 * \return \ref FW_BQ76972_SUCCESS on success, or a \ref FW_BQ76972_SPI_ERR code.
 */
int fw_bq76972_send_subcommand(uint16_t subcommand);

/**
 * \brief Put the device into CONFIG_UPDATE mode.
 *
 * Required before writing to data-memory registers. Pair with
 * \ref fw_bq76972_exit_config_update when done.
 *
 * \return \ref FW_BQ76972_SUCCESS on success, or a \ref FW_BQ76972_SPI_ERR code.
 */
int fw_bq76972_enter_config_update(void);

/**
 * \brief Exit CONFIG_UPDATE mode and resume normal operation.
 *
 * \return \ref FW_BQ76972_SUCCESS on success, or a \ref FW_BQ76972_SPI_ERR code.
 */
int fw_bq76972_exit_config_update(void);

// ============================================================================
// Data Memory Functions
// ============================================================================

/**
 * \brief Write an arbitrary byte array to data memory.
 *
 * \param[in] addr               16-bit data-memory address.
 * \param[in] data               Buffer containing bytes to write.
 * \param[in] len                Number of bytes to write.
 * \param[in] use_config_update  If \c true, enter and exit CONFIG_UPDATE mode
 *                               automatically around the write.
 * \return \ref FW_BQ76972_SUCCESS on success, or a \ref FW_BQ76972_SPI_ERR code.
 */
int fw_bq76972_write_data_memory(uint16_t addr, const uint8_t *data, size_t len,
                              bool use_config_update);

/**
 * \brief Write a single byte to data memory.
 *
 * \param[in] addr               16-bit data-memory address.
 * \param[in] value              Byte value to write.
 * \param[in] use_config_update  If \c true, wraps the write in CONFIG_UPDATE mode.
 * \return \ref FW_BQ76972_SUCCESS on success, or a \ref FW_BQ76972_SPI_ERR code.
 */
int fw_bq76972_write_data_memory_u8(uint16_t addr, uint8_t value,
                                 bool use_config_update);

/**
 * \brief Write a 16-bit little-endian value to data memory.
 *
 * \param[in] addr               16-bit data-memory address.
 * \param[in] value              16-bit value to write (LSB first).
 * \param[in] use_config_update  If \c true, wraps the write in CONFIG_UPDATE mode.
 * \return \ref FW_BQ76972_SUCCESS on success, or a \ref FW_BQ76972_SPI_ERR code.
 */
int fw_bq76972_write_data_memory_u16(uint16_t addr, uint16_t value,
                                  bool use_config_update);

/**
 * \brief Read an arbitrary number of bytes from data memory.
 *
 * \param[in]  addr  16-bit data-memory address.
 * \param[out] data  Buffer to store the read bytes.
 * \param[in]  len   Number of bytes to read.
 * \return \ref FW_BQ76972_SUCCESS on success, or a \ref FW_BQ76972_SPI_ERR code.
 */
int fw_bq76972_read_data_memory(uint16_t addr, uint8_t *data, size_t len);

/**
 * \brief Read a single byte from data memory.
 *
 * \param[in]  addr   16-bit data-memory address.
 * \param[out] value  Pointer to store the byte value.
 * \return \ref FW_BQ76972_SUCCESS on success, or a \ref FW_BQ76972_SPI_ERR code.
 */
int fw_bq76972_read_data_memory_u8(uint16_t addr, uint8_t *value);

/**
 * \brief Read a 16-bit little-endian value from data memory.
 *
 * \param[in]  addr   16-bit data-memory address.
 * \param[out] value  Pointer to store the assembled 16-bit value.
 * \return \ref FW_BQ76972_SUCCESS on success, or a \ref FW_BQ76972_SPI_ERR code.
 */
int fw_bq76972_read_data_memory_u16(uint16_t addr, uint16_t *value);

// ============================================================================
// 16-bit Value Functions (LSB/MSB Read)
// ============================================================================

/**
 * \brief Read a 16-bit value from two consecutive direct-command registers.
 *
 * Issues the command at \p lsb_command, then reads LSB and MSB bytes and
 * assembles them into a single 16-bit result.
 *
 * \param[in]  lsb_command  Direct-command address of the LSB register.
 * \param[out] value        Pointer to store the assembled 16-bit value.
 * \return \ref FW_BQ76972_SUCCESS on success, or a \ref FW_BQ76972_SPI_ERR code.
 */
int fw_bq76972_read_u16(uint8_t lsb_command, uint16_t *value);

// ============================================================================
// Voltage Reading Functions
// ============================================================================

/**
 * \brief Read the voltage of a single cell.
 *
 * \param[in]  cell        Cell register address from \ref fw_cell_voltage_t.
 * \param[out] voltage_mv  Cell voltage in millivolts (1 mV per LSB).
 * \return \ref FW_BQ76972_SUCCESS on success, or a \ref FW_BQ76972_SPI_ERR code.
 */
int fw_bq76972_read_cell_voltage(fw_cell_voltage_t cell, uint16_t *voltage_mv);

/**
 * \brief Read voltages for all cells sequentially.
 *
 * Reads \p num_cells consecutive cell registers starting from \ref FW_CELL_1.
 *
 * \param[out] voltages   Buffer to store cell voltages in millivolts.
 *                        Must hold at least \p num_cells elements.
 * \param[in]  num_cells  Number of cells to read (maximum 16).
 * \return \ref FW_BQ76972_SUCCESS on success, or a \ref FW_BQ76972_SPI_ERR code.
 */
int fw_bq76972_read_all_cell_voltages(uint16_t *voltages, size_t num_cells);

// ============================================================================
// Temperature Reading Functions
// ============================================================================

/**
 * \brief Read the temperature from a single sensor.
 *
 * \param[in]  sensor          Sensor register address from \ref fw_thermistor_t.
 * \param[out] temperature_dK  Temperature in decikelvin (0.1 K per LSB).
 *
 * \note The device reports temperature in 0.1 K units. Use
 *       \ref fw_bq76972_temperature_dK_to_c to convert to degrees Celsius.
 *
 * \return \ref FW_BQ76972_SUCCESS on success, or a \ref FW_BQ76972_SPI_ERR code.
 */
int fw_bq76972_read_temperature(fw_thermistor_t sensor, int16_t *temperature_dK);

/**
 * \brief Read temperatures from multiple sensors.
 *
 * \param[in]  sensors          Array of sensor addresses from \ref fw_thermistor_t.
 * \param[out] temperatures_dK  Buffer to store readings in decikelvin.
 *                              Must hold at least \p num_sensors elements.
 * \param[in]  num_sensors      Number of sensors to read.
 * \return \ref FW_BQ76972_SUCCESS on success, or a \ref FW_BQ76972_SPI_ERR code.
 */
int fw_bq76972_read_all_temperatures(const fw_thermistor_t *sensors,
                                  int16_t *temperatures_dK,
                                  size_t num_sensors);

/**
 * \brief Convert a raw decikelvin temperature to degrees Celsius.
 *
 * \param[in] temperature_dK  Temperature in decikelvin (0.1 K per LSB).
 * \return Temperature in degrees Celsius.
 */
float fw_bq76972_temperature_dK_to_c(int16_t temperature_dK);

// ============================================================================
// Thermistor Pin Configuration
// ============================================================================

/**
 * \brief Configure the function of each thermistor-capable pin.
 *
 * Writes configuration bytes for TS1, TS2, TS3, DFETOFF, DCHG, and DDSG pins
 * to the appropriate data-memory registers. Refer to the BQ76972 datasheet for
 * valid configuration values.
 *
 * \param[in] ts1_config     Configuration byte for the TS1 pin.
 * \param[in] ts2_config     Configuration byte for the TS2 pin.
 * \param[in] ts3_config     Configuration byte for the TS3 pin.
 * \param[in] dfetoff_config Configuration byte for the DFETOFF pin.
 * \param[in] dchg_config    Configuration byte for the DCHG pin.
 * \param[in] ddsg_config    Configuration byte for the DDSG pin.
 * \return \ref FW_BQ76972_SUCCESS on success, or a \ref FW_BQ76972_SPI_ERR code.
 */
int fw_bq76972_configure_thermistors(uint8_t ts1_config,
                                  uint8_t ts2_config,
                                  uint8_t ts3_config,
                                  uint8_t dfetoff_config,
                                  uint8_t dchg_config,
                                  uint8_t ddsg_config);

/**
 * \brief Apply the default thermistor pin configuration for this firmware.
 *
 * Convenience wrapper around \ref fw_bq76972_configure_thermistors using
 * the firmware's standard thermistor assignments.
 *
 * \return \ref FW_BQ76972_SUCCESS on success, or a \ref FW_BQ76972_SPI_ERR code.
 */
int fw_bq76972_configure_thermistors_default(void);

/**
 * \brief Read back the current thermistor pin configuration from the device.
 *
 * \param[out] ts1_config     Configuration byte for the TS1 pin.
 * \param[out] ts2_config     Configuration byte for the TS2 pin.
 * \param[out] ts3_config     Configuration byte for the TS3 pin.
 * \param[out] dfetoff_config Configuration byte for the DFETOFF pin.
 * \param[out] dchg_config    Configuration byte for the DCHG pin.
 * \param[out] ddsg_config    Configuration byte for the DDSG pin.
 * \return \ref FW_BQ76972_SUCCESS on success, or a \ref FW_BQ76972_SPI_ERR code.
 */
int fw_bq76972_read_thermistor_pin_configs(uint8_t *ts1_config,
                                        uint8_t *ts2_config,
                                        uint8_t *ts3_config,
                                        uint8_t *dfetoff_config,
                                        uint8_t *dchg_config,
                                        uint8_t *ddsg_config);

/**
 * \brief Read temperatures from all configured thermistor pins.
 *
 * Reads \p num_temperatures values from the thermistor pins that have been
 * enabled as temperature inputs (TS1, TS2, TS3, DFETOFF, DCHG, DDSG).
 *
 * \param[out] temperatures_dK  Buffer to store readings in decikelvin.
 *                              Must hold at least \p num_temperatures elements.
 * \param[in]  num_temperatures Number of thermistor values to read.
 * \return \ref FW_BQ76972_SUCCESS on success, or a \ref FW_BQ76972_SPI_ERR code.
 */
int fw_bq76972_read_all_thermistor_values(int16_t *temperatures_dK,
                                       size_t num_temperatures);

// ============================================================================
// Current Reading Functions
// ============================================================================

/**
 * \brief Selectable current-measurement resolution via the USER_AMPS setting
 *        in the DA Configuration register (0x9303).
 */
typedef enum
{
    FW_USER_AMPS_0_1_MA = 0, /**< 0.1 mA per count  */
    FW_USER_AMPS_1_MA   = 1, /**< 1 mA per count (device default) */
    FW_USER_AMPS_10_MA  = 2, /**< 10 mA per count  */
    FW_USER_AMPS_100_MA = 3, /**< 100 mA per count */
} fw_user_amps_t;

/**
 * \brief Write the USER_AMPS resolution setting to data memory.
 *
 * \param[in] setting  Desired resolution from \ref fw_user_amps_t.
 * \return \ref FW_BQ76972_SUCCESS on success, or a \ref FW_BQ76972_SPI_ERR code.
 */
int fw_bq76972_configure_user_amps(fw_user_amps_t setting);

/**
 * \brief Read the current USER_AMPS resolution setting from data memory.
 *
 * \param[out] setting  Pointer to store the current setting.
 * \return \ref FW_BQ76972_SUCCESS on success, or a \ref FW_BQ76972_SPI_ERR code.
 */
int fw_bq76972_read_user_amps(fw_user_amps_t *setting);

/**
 * \brief Read the raw CC2 current register value.
 *
 * The returned value is a signed 16-bit integer whose units depend on the
 * active \ref fw_user_amps_t setting. Use \ref fw_bq76972_read_current_mA for
 * a milliamp-scaled result.
 *
 * \param[out] raw  Pointer to store the raw signed CC2 count.
 * \return \ref FW_BQ76972_SUCCESS on success, or a \ref FW_BQ76972_SPI_ERR code.
 */
int fw_bq76972_read_current_raw(int16_t *raw);

/**
 * \brief Read the CC2 current and convert to milliamps.
 *
 * Reads the raw CC2 register and applies the active \ref fw_user_amps_t
 * scaling factor. Call \ref fw_bq76972_configure_user_amps before this
 * function, otherwise the device default (1 mA per count) is assumed.
 *
 * \param[out] current_mA  Pack current in milliamps (negative = discharge).
 * \return \ref FW_BQ76972_SUCCESS on success, or a \ref FW_BQ76972_SPI_ERR code.
 */
int fw_bq76972_read_current_mA(int32_t *current_mA);

// ============================================================================
// Configuration Verification
// ============================================================================

/**
 * \brief Verify that thermistor pin configuration matches expected values.
 *
 * Reads back the thermistor pin registers and compares them against the
 * firmware's expected configuration.
 *
 * \return \ref FW_BQ76972_SUCCESS if configuration matches, or a
 *         \ref FW_BQ76972_SPI_ERR code on communication failure.
 */
int fw_bq76972_verify_thermistor_config(void);

// ============================================================================
// Power Management
// ============================================================================

/**
 * \brief Wake the BQ76972 from sleep or shutdown mode.
 *
 * Drives the wake-up sequence required by the device before issuing any
 * SPI commands after a low-power state.
 */
void fw_bq76972_wakeup(void);

#endif // FW_BQ76972_H
