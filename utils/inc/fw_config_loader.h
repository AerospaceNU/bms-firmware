#ifndef FW_CONFIG_LOADER_H
#define FW_CONFIG_LOADER_H

//*************************************************************************************************
// Configuration Loader Module
//*************************************************************************************************

#include "fw_config.h"

void fw_config_all(uint8_t pullup, uint8_t model, uint8_t meas_type);
void fw_config_thermistors(uint therm, uint8_t pullup, uint8_t model, uint8_t meas_type);


//*************************************************************************************************

#endif // FW_CONFIG_LOADER_H