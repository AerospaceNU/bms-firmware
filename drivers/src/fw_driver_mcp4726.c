#include "fw_driver_mcp4726.h"

#include <stdio.h>

#include "pico/error.h"

bool fw_driver_mcp4726_init(fw_mcp4726_t *dev, i2c_inst_t *i2c, uint8_t addr, float vref)
{
    if (dev == NULL || i2c == NULL)
    {
        return false;
    }

    dev->i2c = i2c;
    dev->addr = addr;
    dev->vref = vref;
    dev->current_code = 0;

    return true;
}

bool fw_driver_mcp4726_set_voltage(fw_mcp4726_t *dev, float volts)
{
    if (dev == NULL)
    {
        return false;
    }

    if (volts < 0.0f)
    {
        volts = 0.0f;
    }
    if (volts > dev->vref)
    {
        volts = dev->vref;
    }

    uint16_t code = (uint16_t)((volts / dev->vref) * FW_MCP4726_MAX_CODE);

    return fw_driver_mcp4726_set_code(dev, code);
}

bool fw_driver_mcp4726_set_code(fw_mcp4726_t *dev, uint16_t code)
{
    if (dev == NULL)
    {
        return false;
    }

    if (code > FW_MCP4726_MAX_CODE)
    {
        code = FW_MCP4726_MAX_CODE;
    }

    uint8_t buf[2];
    buf[0] = (uint8_t)((code >> 8) & 0x0Fu);
    buf[1] = (uint8_t)(code & 0xFFu);

    int result = i2c_write_blocking(dev->i2c, dev->addr, buf, 2, false);
    if (result == PICO_ERROR_GENERIC)
    {
        return false;
    }

    dev->current_code = code;
    return true;
}

bool fw_driver_mcp4726_set_powerdown(fw_mcp4726_t *dev, fw_mcp4726_pwrdn_mode_t mode)
{
    if (dev == NULL)
    {
        return false;
    }

    uint8_t buf[2];
    buf[0] = (uint8_t)(((mode & 0x03u) << 4) | ((dev->current_code >> 8) & 0x0Fu));
    buf[1] = (uint8_t)(dev->current_code & 0xFFu);

    int result = i2c_write_blocking(dev->i2c, dev->addr, buf, 2, false);
    if (result == PICO_ERROR_GENERIC)
    {
        return false;
    }

    return true;
}

bool fw_driver_mcp4726_read_eeprom(fw_mcp4726_t *dev, uint16_t *code)
{
    if (dev == NULL || code == NULL)
    {
        return false;
    }

    uint8_t buf[3];
    int result = i2c_read_blocking(dev->i2c, dev->addr, buf, 3, false);
    if (result == PICO_ERROR_GENERIC)
    {
        return false;
    }

    *code = (uint16_t)(((buf[1] & 0x0Fu) << 8) | buf[2]);
    dev->current_code = *code;

    return true;
}

uint16_t fw_driver_mcp4726_get_current_code(const fw_mcp4726_t *dev)
{
    if (dev == NULL)
    {
        return 0;
    }

    return dev->current_code;
}

float fw_driver_mcp4726_get_voltage(const fw_mcp4726_t *dev)
{
    if (dev == NULL)
    {
        return 0.0f;
    }

    return (dev->current_code / (float)FW_MCP4726_MAX_CODE) * dev->vref;
}
