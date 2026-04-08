#ifndef FW_DRIVER_MCP4726_H
#define FW_DRIVER_MCP4726_H

#include <stdbool.h>
#include <stdint.h>

#include "hardware/i2c.h"

//*************************************************************************************************
// MCP4726 Configuration
//*************************************************************************************************

#define FW_MCP4726_ADDR_DEFAULT 0x60u
#define FW_MCP4726_MAX_CODE 4095u
#define FW_MCP4726_VREF_DEFAULT 3.3f

// Command byte format bits
#define FW_MCP4726_CMD_FAST_MODE 0x00u
#define FW_MCP4726_CMD_EEPROM 0x60u

typedef enum
{
    FW_MCP4726_PWRDN_NORMAL = 0,
    FW_MCP4726_PWRDN_1K = 1,
    FW_MCP4726_PWRDN_100K = 2,
    FW_MCP4726_PWRDN_500K = 3
} fw_mcp4726_pwrdn_mode_t;

typedef enum
{
    FW_MCP4726_VREF_VDD = 0,
    FW_MCP4726_VREF_INTERNAL = 1
} fw_mcp4726_vref_t;

typedef enum
{
    FW_MCP4726_GAIN_1X = 0,
    FW_MCP4726_GAIN_2X = 1
} fw_mcp4726_gain_t;

typedef struct
{
    i2c_inst_t *i2c;
    uint8_t addr;
    float vref;
    uint16_t current_code;
} fw_mcp4726_t;

bool fw_driver_mcp4726_init(fw_mcp4726_t *dev, i2c_inst_t *i2c, uint8_t addr, float vref);
bool fw_driver_mcp4726_set_voltage(fw_mcp4726_t *dev, float volts);
bool fw_driver_mcp4726_set_code(fw_mcp4726_t *dev, uint16_t code);
bool fw_driver_mcp4726_set_powerdown(fw_mcp4726_t *dev, fw_mcp4726_pwrdn_mode_t mode);
bool fw_driver_mcp4726_read_eeprom(fw_mcp4726_t *dev, uint16_t *code);
uint16_t fw_driver_mcp4726_get_current_code(const fw_mcp4726_t *dev);
float fw_driver_mcp4726_get_voltage(const fw_mcp4726_t *dev);

#endif // FW_DRIVER_MCP4726_H
