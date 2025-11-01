/* Global Includes */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/* Local Includes */
#include "fw_temperature.h"
#include "fw_config_loader.h"
#include "fw_config.h"
#include "fw_bq76972.h"

//*************************************************************************************************

/**
 * @brief Initialize Temperature Sensors to default configuration (specified in header)
 * @return error code (or 1 if successful)
 */
int fw_therm_init() {
    LOG_INFO("Initializing Temperature Sensors...");
    // Configure thermistors with default settings
    int err = fw_therm_config_all(FW_THERM_PULLUP, FW_THERM_MODEL, FW_THERM_MEAS_TYPE);
    if (err != 1) {
        LOG_ERROR("Failed to configure thermistors during initialization");
        return err;
    }
    LOG_INFO("Temperature Sensors initialized successfully");
    return 1;
}

/**
 * @brief Deinitialize Temperature Sensors (no specific deinitialization required)
 * @return error code (or 1 if successful)
 */
int fw_therm_deinit() {
    LOG_INFO("Deinitializing Temperature Sensors...");
    // No specific deinitialization required for thermistors
    LOG_INFO("Temperature Sensors deinitialized successfully");
    return 1;
}

/**
 * @brief Read all thermistor temperatures
 * @param temperatures Pointer to array to store temperatures (size should be at least FW_NUM_THERMISTORS)
 * @return error code (or 1 if successful)
 */
int fw_therm_read_all(float *temperatures) {
    LOG_INFO("Reading all thermistor temperatures...");
    uint8_t *therm_addresses = {FW_THERM_1, FW_THERM_2, FW_THERM_3, FW_THERM_4, FW_THERM_5, FW_THERM_6};

    for (int i = 0; i < 6; i++) {
        int err = fw_therm_read(therm_addresses[i], &temperatures[i]);
        if (err != 1) {
            LOG_ERROR("Failed to read temperature from thermistor at address 0x%02X", therm_addresses[i]);
            return err;
        }
    }
    return 1;
}

/**
 * @brief Read temperature from a specific thermistor
 * @param therm_addr Thermistor address (FW_THERM_1, FW_THERM_2, FW_THERM_3, FW_THERM_4, FW_THERM_5, FW_THERM_6)
 * @param temperature Pointer to store the read temperature
 * @return error code (or 1 if successful)
 */
int fw_therm_read(uint8_t therm_addr, float *temperature) {
    LOG_INFO("Reading temperature from thermistor at address 0x%02X...", therm_addr);
    uint8_t raw_data[2];
    int err = fw_bq76971_read_register(therm_addr, raw_data, 3);
    if (err != 1) {
        LOG_ERROR("Failed to read raw data from thermistor at address 0x%02X", therm_addr);
        return err;
    }
    uint16_t raw_temp = (uint16_t)(raw_data[1] << 8) | raw_data[0];
    // convert raw_temp to celsius from 0.1K kelvin
    *temperature = ((float)raw_temp * 0.1f) - 273.15f;
    LOG_INFO("Thermistor at address 0x%02X temperature: %.2f °C", therm_addr, *temperature);
    return 1;
}

int fw_therm_read_internal(float *temperature) {
    LOG_DEBUG("Reading internal die temperature...");
    int err = fw_therm_read(FW_THERM_INTERNAL, temperature);
    if (err != 1) {
        LOG_ERROR("Failed to read internal die temperature");
        return err;
    }
    return 1;
}