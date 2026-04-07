#include "drivers/inc/fw_driver_smbus.h"

#include "hardware/i2c.h"
#include "pico/stdlib.h"

#define FW_SMBUS_PORT i2c0
#define FW_SMBUS_SDA 8
#define FW_SMBUS_SCL 9

#define BATT_VOLTAGE_MV 15268
#define BATT_CURRENT_MA -6700
#define BATT_TEMP_DK 2981
#define BATT_REMAINING_MAH 4000
#define BATT_FULL_MAH 5000
#define BATT_STATE_OF_CHARGE 80
#define BATT_CYCLE_COUNT 12

#define CELL1_MV 3676
#define CELL2_MV 3696
#define CELL3_MV 4200
#define CELL4_MV 3696

static uint8_t g_current_register = 0x00;
static uint8_t g_byte_index = 0;

static uint8_t fw_driver_smbus_get_register_byte(uint8_t reg, uint8_t byte_idx)
{
    uint16_t value = 0;

    switch (reg)
    {
    case 0x08:
        value = BATT_TEMP_DK;
        break;
    case 0x09:
        value = BATT_VOLTAGE_MV;
        break;
    case 0x0A:
        value = (uint16_t)BATT_CURRENT_MA;
        break;
    case 0x0D:
        value = BATT_STATE_OF_CHARGE;
        break;
    case 0x0F:
        value = BATT_REMAINING_MAH;
        break;
    case 0x10:
        value = BATT_FULL_MAH;
        break;
    case 0x13:
        value = BATT_CYCLE_COUNT;
        break;

    // Cell voltages read by ArduPilot SMBus Generic from 0x3F down to 0x34.
    case 0x3F:
        value = CELL1_MV;
        break;
    case 0x3E:
        value = CELL2_MV;
        break;
    case 0x3D:
        value = CELL3_MV;
        break;
    case 0x3C:
        value = CELL4_MV;
        break;

    default:
        value = 0x0000;
        break;
    }

    if (byte_idx == 0)
    {
        return (uint8_t)(value & 0xFFu);
    }

    return (uint8_t)((value >> 8) & 0xFFu);
}

static void fw_driver_smbus_irq_handler(void)
{
    i2c_hw_t *i2c_hw = i2c_get_hw(FW_SMBUS_PORT);
    uint32_t status = i2c_hw->intr_stat;

    if ((status & I2C_IC_INTR_STAT_R_RX_FULL_BITS) != 0u)
    {
        g_current_register = (uint8_t)i2c_hw->data_cmd;
        g_byte_index = 0;
    }

    if ((status & I2C_IC_INTR_STAT_R_RD_REQ_BITS) != 0u)
    {
        i2c_hw->data_cmd = fw_driver_smbus_get_register_byte(g_current_register, g_byte_index);
        (void)i2c_hw->clr_rd_req;
        g_byte_index++;
    }
}

int fw_driver_smbus_init(uint8_t slave_addr)
{
    i2c_init(FW_SMBUS_PORT, 100 * 1000);

    gpio_set_function(FW_SMBUS_SDA, GPIO_FUNC_I2C);
    gpio_set_function(FW_SMBUS_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(FW_SMBUS_SDA);
    gpio_pull_up(FW_SMBUS_SCL);

    i2c_set_slave_mode(FW_SMBUS_PORT, true, slave_addr);

    irq_set_exclusive_handler(I2C0_IRQ, fw_driver_smbus_irq_handler);
    irq_set_enabled(I2C0_IRQ, true);

    i2c_get_hw(FW_SMBUS_PORT)->intr_mask =
        I2C_IC_INTR_MASK_M_RD_REQ_BITS |
        I2C_IC_INTR_MASK_M_RX_FULL_BITS;

    return 0;
}
