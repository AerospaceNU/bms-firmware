/* Global Includes */
#include <stdlib.h>
#include <stdio.h>

/* Local Includes */
#include "fw_config_loader.h"
#include "fw_config.h"
#include "fw_bq76972.h"
#include "fw_util_logging.h"

//*************************************************************************************************
// Thermistor Configuration Functions
//*************************************************************************************************

/**
 * @brief Configure all thermistors with same settings
 * @param pullup Pullup resistor setting (FW_PULLUP_18K, FW_PULLUP_180K, FW_NO_PULLUP)
 * @param model Thermistor model setting (FW_MODEL_18K, FW_MODEL_180K, FW_MODEL_CUSTOM, FW_MODEL_NONE)
 * @param meas_type Measurement type setting (FW_MEAS_TYPE_GEN_ADCIN, FW_MEAS_TYPE_CELL_TEMP_PROT, FW_MEAS_TYPE_CELL_TEMP, FW_MEAS_TYPE_FET_TEMP_PROT)
 */
int fw_therm_config_all(uint8_t pullup, uint8_t model, uint8_t meas_type)
{
    const uint8_t thermistors[] = {FW_THERM_1, FW_THERM_2, FW_THERM_3, FW_THERM_4, FW_THERM_5, FW_THERM_6};
    char msg[120];
    snprintf(msg, sizeof(msg), "Configuring all thermistors with pullup: 0x%02X, model: 0x%02X, meas_type: 0x%02X", pullup, model, meas_type);
    LOG_DEBUG(msg);
    for (int i = 0; i < 6; i++)
    {
        int err = fw_therm_config(thermistors[i], pullup, model, meas_type);
        if (err != 1)
        {
            return err;
        }
    }

    return 1;
}

/**
 * @brief Configure a specific thermistor
 * @param therm Thermistor address (FW_THERM_1, FW_THERM_2, FW_THERM_3, FW_THERM_4, FW_THERM_5, FW_THERM_6)
 * @param pullup Pullup resistor setting (FW_PULLUP_18K, FW_PULLUP_180K, FW_NO_PULLUP)
 * @param model Thermistor model setting (FW_MODEL_18K, FW_MODEL_180K, FW_MODEL_CUSTOM, FW_MODEL_NONE)
 * @param meas_type Measurement type setting (FW_MEAS_TYPE_GEN_ADCIN, FW_MEAS_TYPE_CELL_TEMP_PROT, FW_MEAS_TYPE_CELL_TEMP, FW_MEAS_TYPE
 */
int fw_therm_config(uint8_t therm, uint8_t pullup, uint8_t model, uint8_t meas_type)
{
    uint8_t config_value = 0 | FW_FXN_THERM | pullup | model | meas_type;
    char msg[80];
    int err = fw_bq76971_write_register(therm, &config_value, 1);
    if (err != 1)
    {
        snprintf(msg, sizeof(msg), "Failed to configure thermistor at address 0x%02X", therm);
        LOG_ERROR(msg);
        return err;
    }
    snprintf(msg, sizeof(msg), "Thermistor at address 0x%02X configured successfully", therm);
    LOG_INFO(msg);

    return 1;
}

//*************************************************************************************************
// _________________ Configuration Module
//*************************************************************************************************
