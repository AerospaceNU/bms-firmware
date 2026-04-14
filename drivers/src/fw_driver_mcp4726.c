#include "fw_driver_mcp4726.h"

#include <stdio.h>

#include "pico/error.h"
#include "fw_util_logging.h"

bool fw_driver_mcp4726_init(fw_mcp4726_t *dev, i2c_inst_t *i2c, uint8_t addr, float vref)
{
    char msg[96] = {0};

    if (dev == NULL || i2c == NULL)
    {
        LOG_ERROR("MCP4726 init failed: null pointer input");
        return false;
    }

    dev->i2c = i2c;
    dev->addr = addr;
    dev->vref = vref;
    dev->current_code = 0;

    snprintf(msg, sizeof(msg), "MCP4726 initialized addr=0x%02X vref=%.3f", addr, (double)vref);
    LOG_MESSAGE(msg);

    return true;
}

bool fw_driver_mcp4726_set_voltage(fw_mcp4726_t *dev, float volts)
{
    char msg[96] = {0};

    if (dev == NULL)
    {
        LOG_ERROR("MCP4726 set_voltage failed: dev is NULL");
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

    snprintf(msg, sizeof(msg), "MCP4726 set_voltage request=%.3fV code=%u", (double)volts, (unsigned)code);
    LOG_DEBUG(msg);

    return fw_driver_mcp4726_set_code(dev, code);
}

bool fw_driver_mcp4726_set_code(fw_mcp4726_t *dev, uint16_t code)
{
    char msg[96] = {0};

    if (dev == NULL)
    {
        LOG_ERROR("MCP4726 set_code failed: dev is NULL");
        return false;
    }

    if (code > FW_MCP4726_MAX_CODE)
    {
        code = FW_MCP4726_MAX_CODE;
    }

    uint8_t buf[2];
    buf[0] = (uint8_t)((code >> 8) & 0x0Fu);
    buf[1] = (uint8_t)(code & 0xFFu);

    snprintf(msg, sizeof(msg), "MCP4726 TX set_code code=%u bytes=%02X%02X", (unsigned)code, buf[0], buf[1]);
    LOG_DEBUG(msg);

    int result = i2c_write_blocking(dev->i2c, dev->addr, buf, 2, false);
    if (result == PICO_ERROR_GENERIC)
    {
        LOG_ERROR("MCP4726 set_code I2C write failed");
        return false;
    }

    dev->current_code = code;
    LOG_DEBUG("MCP4726 set_code success");
    return true;
}

bool fw_driver_mcp4726_set_powerdown(fw_mcp4726_t *dev, fw_mcp4726_pwrdn_mode_t mode)
{
    char msg[96] = {0};

    if (dev == NULL)
    {
        LOG_ERROR("MCP4726 set_powerdown failed: dev is NULL");
        return false;
    }

    uint8_t buf[2];
    buf[0] = (uint8_t)(((mode & 0x03u) << 4) | ((dev->current_code >> 8) & 0x0Fu));
    buf[1] = (uint8_t)(dev->current_code & 0xFFu);

    snprintf(msg,
             sizeof(msg),
             "MCP4726 TX set_powerdown mode=%u bytes=%02X%02X",
             (unsigned)(mode & 0x03u),
             buf[0],
             buf[1]);
    LOG_DEBUG(msg);

    int result = i2c_write_blocking(dev->i2c, dev->addr, buf, 2, false);
    if (result == PICO_ERROR_GENERIC)
    {
        LOG_ERROR("MCP4726 set_powerdown I2C write failed");
        return false;
    }

    LOG_DEBUG("MCP4726 set_powerdown success");
    return true;
}

bool fw_driver_mcp4726_read_eeprom(fw_mcp4726_t *dev, uint16_t *code)
{
    char msg[96] = {0};

    if (dev == NULL || code == NULL)
    {
        LOG_ERROR("MCP4726 read_eeprom failed: null pointer input");
        return false;
    }

    uint8_t buf[3];
    int result = i2c_read_blocking(dev->i2c, dev->addr, buf, 3, false);
    if (result == PICO_ERROR_GENERIC)
    {
        LOG_ERROR("MCP4726 read_eeprom I2C read failed");
        return false;
    }

    *code = (uint16_t)(((buf[1] & 0x0Fu) << 8) | buf[2]);
    dev->current_code = *code;

    snprintf(msg, sizeof(msg), "MCP4726 RX eeprom code=%u bytes=%02X%02X%02X", (unsigned)*code, buf[0], buf[1], buf[2]);
    LOG_DEBUG(msg);

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
