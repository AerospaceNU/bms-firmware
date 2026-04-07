#ifndef FW_CONFIG_LOADER_H
#define FW_CONFIG_LOADER_H

//*************************************************************************************************
// Configuration Loader Module
//*************************************************************************************************

#include "fw_config.h"
#include "fw_bq76972.h"
#include <stdint.h>

int fw_therm_config_all(uint8_t pullup, uint8_t model, uint8_t meas_type);
int fw_therm_config(thermistor_t therm, uint8_t pullup, uint8_t model, uint8_t meas_type);

//*************************************************************************************************

#endif // FW_CONFIG_LOADER_H