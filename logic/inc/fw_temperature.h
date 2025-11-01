//*************************************************************************************************
//  Module                  :   fw_logic_temperature.h
//  Revision                :   0.1
//  Description             :   Read and configure temperature sensors
//*************************************************************************************************
#ifndef _FW_TEMPERATURE_H_
#define _FW_TEMPERATURE_H_


#include "fw_config.h"

/*************************************************************************************************/

// Thermistor Initial Configurations
#define FW_THERM_PULLUP                     FW_18K_PULLUP       // Set default pullup resistor
#define FW_THERM_MODEL                      FW_18K_MODEL        // Set default thermistor model
#define FW_THERM_MEAS_TYPE                  FW_CELL_TEMP_PROT   // Set default measurement type

/*************************************************************************************************/

int fw_therm_init();
int fw_therm_deinit();
int fw_therm_read_all(float *temperatures);
int fw_therm_read(uint8_t therm_addr, float *temperature);

#endif // _FW_TEMPERATURE_H_