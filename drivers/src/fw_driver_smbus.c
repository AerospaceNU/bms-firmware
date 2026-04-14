#include "drivers/inc/fw_driver_smbus.h"

#include "drivers/inc/fw_driver_bq76972.h"

#include <stdio.h>

#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "utils/inc/fw_util_logging.h"

#define FW_SMBUS_PORT i2c0
#define FW_SMBUS_SDA 8
#define FW_SMBUS_SCL 9

#define FW_SMBUS_BATTERY_CELL_COUNT 16

typedef struct
{
    volatile uint16_t pack_voltage_mv;
    volatile int32_t current_ma;
    volatile uint16_t temperature_dK;
    volatile uint16_t remaining_mah;
    volatile uint16_t full_mah;
    volatile uint8_t state_of_charge;
    volatile uint16_t cycle_count;
    volatile uint16_t cell_voltage_mv[FW_SMBUS_BATTERY_CELL_COUNT];
    volatile bool valid;
} fw_driver_smbus_snapshot_t;

static uint8_t g_current_register = 0x00;
static uint8_t g_byte_index = 0;
static fw_driver_smbus_snapshot_t g_snapshot = {
    .pack_voltage_mv = 0,
    .current_ma = 0,
    .temperature_dK = 0,
    .remaining_mah = 0,
    .full_mah = 0,
    .state_of_charge = 0,
    .cycle_count = 0,
    .cell_voltage_mv = {0},
    .valid = false,
};

static uint16_t fw_driver_smbus_sum_cells(const uint16_t *cells, size_t count)
{
    uint32_t total = 0;

    for (size_t i = 0; i < count; i++)
    {
        total += cells[i];
    }

    if (total > UINT16_MAX)
    {
        return UINT16_MAX;
    }

    return (uint16_t)total;
}

static uint16_t fw_driver_smbus_pack_u16(uint8_t reg, uint8_t byte_idx, uint16_t value)
{
    if (byte_idx == 0)
    {
        return (uint16_t)(value & 0x00FFu);
    }

    (void)reg;
    return (uint16_t)((value >> 8) & 0x00FFu);
}

static uint8_t fw_driver_smbus_get_register_byte(uint8_t reg, uint8_t byte_idx)
{
    uint16_t value = 0;

    switch (reg)
    {
    case 0x08:
        value = g_snapshot.valid ? g_snapshot.temperature_dK : 0;
        break;
    case 0x09:
        value = g_snapshot.valid ? g_snapshot.pack_voltage_mv : 0;
        break;
    case 0x0A:
        value = (uint16_t)g_snapshot.current_ma;
        break;
    case 0x0D:
        value = g_snapshot.state_of_charge;
        break;
    case 0x0F:
        value = g_snapshot.remaining_mah;
        break;
    case 0x10:
        value = g_snapshot.full_mah;
        break;
    case 0x13:
        value = g_snapshot.cycle_count;
        break;

    // Cell voltages are exposed from 0x3F down to 0x30.
    case 0x3F:
        value = g_snapshot.cell_voltage_mv[0];
        break;
    case 0x3E:
        value = g_snapshot.cell_voltage_mv[1];
        break;
    case 0x3D:
        value = g_snapshot.cell_voltage_mv[2];
        break;
    case 0x3C:
        value = g_snapshot.cell_voltage_mv[3];
        break;
    case 0x3B:
        value = g_snapshot.cell_voltage_mv[4];
        break;
    case 0x3A:
        value = g_snapshot.cell_voltage_mv[5];
        break;
    case 0x39:
        value = g_snapshot.cell_voltage_mv[6];
        break;
    case 0x38:
        value = g_snapshot.cell_voltage_mv[7];
        break;
    case 0x37:
        value = g_snapshot.cell_voltage_mv[8];
        break;
    case 0x36:
        value = g_snapshot.cell_voltage_mv[9];
        break;
    case 0x35:
        value = g_snapshot.cell_voltage_mv[10];
        break;
    case 0x34:
        value = g_snapshot.cell_voltage_mv[11];
        break;
    case 0x33:
        value = g_snapshot.cell_voltage_mv[12];
        break;
    case 0x32:
        value = g_snapshot.cell_voltage_mv[13];
        break;
    case 0x31:
        value = g_snapshot.cell_voltage_mv[14];
        break;
    case 0x30:
        value = g_snapshot.cell_voltage_mv[15];
        break;

    default:
        value = 0x0000;
        break;
    }

    return (uint8_t)fw_driver_smbus_pack_u16(reg, byte_idx, value);
}

int fw_driver_smbus_refresh(void)
{
    char msg[128] = {0};
    uint16_t cells[FW_SMBUS_BATTERY_CELL_COUNT] = {0};
    uint16_t pack_voltage_mv = 0;
    int32_t current_ma = 0;
    int16_t temperature_dK = 0;

    LOG_DEBUG("SMBus refresh started");

    if (fw_bq76972_read_all_cell_voltages(cells, FW_SMBUS_BATTERY_CELL_COUNT) != 1)
    {
        LOG_ERROR("SMBus refresh failed: read_all_cell_voltages");
        return -1;
    }

    if (fw_bq76972_read_cell_voltage(FW_STACK_VOLTAGE, &pack_voltage_mv) != FW_BQ76972_SUCCESS)
    {
        pack_voltage_mv = fw_driver_smbus_sum_cells(cells, FW_SMBUS_BATTERY_CELL_COUNT);
        snprintf(msg, sizeof(msg), "SMBus refresh fallback: summed pack voltage = %u mV", (unsigned)pack_voltage_mv);
        LOG_WARNING(msg);
    }

    if (fw_bq76972_read_current_mA(&current_ma) != FW_BQ76972_SUCCESS)
    {
        current_ma = 0;
        LOG_WARNING("SMBus refresh fallback: current = 0 mA");
    }

    if (fw_bq76972_read_temperature(FW_INT_THERMISTOR, &temperature_dK) != FW_BQ76972_SUCCESS)
    {
        temperature_dK = 0;
        LOG_WARNING("SMBus refresh fallback: temperature = 0 dK");
    }

    g_snapshot.pack_voltage_mv = pack_voltage_mv;
    g_snapshot.current_ma = current_ma;
    g_snapshot.temperature_dK = (uint16_t)temperature_dK;
    g_snapshot.remaining_mah = 4000;
    g_snapshot.full_mah = 5000;
    g_snapshot.state_of_charge = 80;
    g_snapshot.cycle_count = 12;

    for (size_t i = 0; i < FW_SMBUS_BATTERY_CELL_COUNT; i++)
    {
        g_snapshot.cell_voltage_mv[i] = cells[i];
    }

    g_snapshot.valid = true;

    snprintf(msg,
             sizeof(msg),
             "SMBus refresh complete: V=%u mV I=%ld mA T=%u dK SoC=%u%%",
             (unsigned)g_snapshot.pack_voltage_mv,
             (long)g_snapshot.current_ma,
             (unsigned)g_snapshot.temperature_dK,
             (unsigned)g_snapshot.state_of_charge);
    LOG_DEBUG(msg);

    return 0;
}

static void fw_driver_smbus_irq_handler(void)
{
    char msg[96] = {0};
    i2c_hw_t *i2c_hw = i2c_get_hw(FW_SMBUS_PORT);
    uint32_t status = i2c_hw->intr_stat;

    if ((status & I2C_IC_INTR_STAT_R_RX_FULL_BITS) != 0u)
    {
        g_current_register = (uint8_t)i2c_hw->data_cmd;
        g_byte_index = 0;

        snprintf(msg, sizeof(msg), "SMBus RX register select: 0x%02X", g_current_register);
        LOG_DEBUG(msg);
    }

    if ((status & I2C_IC_INTR_STAT_R_RD_REQ_BITS) != 0u)
    {
        uint8_t tx_byte = fw_driver_smbus_get_register_byte(g_current_register, g_byte_index);
        i2c_hw->data_cmd = tx_byte;
        (void)i2c_hw->clr_rd_req;

        snprintf(msg,
                 sizeof(msg),
                 "SMBus TX reg=0x%02X byte=%u value=0x%02X",
                 g_current_register,
                 (unsigned)g_byte_index,
                 tx_byte);
        LOG_DEBUG(msg);

        g_byte_index++;
    }
}

int fw_driver_smbus_init(uint8_t slave_addr)
{
    char msg[80] = {0};
    LOG_MESSAGE("Initializing SMBus slave");

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

    g_snapshot.valid = false;

    snprintf(msg, sizeof(msg), "SMBus slave initialized at address 0x%02X", slave_addr);
    LOG_MESSAGE(msg);

    return 0;
}
