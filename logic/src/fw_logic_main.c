/* Global Includes */
#include <stdio.h>

/* Local Includes */
#include "hardware/i2c.h"
#include "pico/stdlib.h"

#include "utils/inc/fw_constants.h"
#include "utils/inc/fw_types.h"
#include "utils/inc/fw_util_logging.h"
#include "utils/inc/fw_globals.h"
#include "logic/inc/fw_logic_error.h"
#include "drivers/inc/fw_driver_bq76972.h"
#include "drivers/inc/fw_driver_mcp4726.h"
#include "drivers/inc/fw_driver_smbus.h"

#define FW_MCP4726_PORT i2c1
#define FW_MCP4726_SDA 14
#define FW_MCP4726_SCL 15
#define FW_MCP4726_ADDR FW_MCP4726_ADDR_DEFAULT

static fw_mcp4726_t g_mcp4726;

int fw_logic_init()
{
  if (stdio_init_all() == false)
  {
    LOG_ERROR("Could not initialize stdio");
    return E_FW_LOGIC_INIT;
  }

  if (fw_driver_smbus_init(0x0B) != 0)
  {
    LOG_ERROR("Could not initialize I2C slave");
    return E_FW_LOGIC_INIT;
  }

  fw_bq76972_init();

  if (fw_bq76972_configure_thermistors_default() != FW_BQ76972_SUCCESS)
  {
    LOG_ERROR("Could not configure BQ76972 thermistors");
    return E_BQ76972(1);
  }

  if (fw_bq76972_configure_user_amps(FW_USER_AMPS_1_MA) != FW_BQ76972_SUCCESS)
  {
    LOG_ERROR("Could not configure BQ76972 current scaling");
    return E_BQ76972(2);
  }

  fw_driver_smbus_refresh();

  i2c_init(FW_MCP4726_PORT, 100 * 1000);
  gpio_set_function(FW_MCP4726_SDA, GPIO_FUNC_I2C);
  gpio_set_function(FW_MCP4726_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(FW_MCP4726_SDA);
  gpio_pull_up(FW_MCP4726_SCL);

  if (!fw_driver_mcp4726_init(&g_mcp4726, FW_MCP4726_PORT, FW_MCP4726_ADDR, FW_MCP4726_VREF_DEFAULT))
  {
    LOG_ERROR("Could not initialize MCP4726 DAC");
    return E_FW_LOGIC_INIT;
  }

  if (!fw_driver_mcp4726_set_voltage(&g_mcp4726, 0.0f))
  {
    LOG_ERROR("Could not set MCP4726 DAC output");
    return E_FW_LOGIC_INIT;
  }

  fw_time_init(&g_current_time);
  LOG_DEBUG("g_current_time initialized");
  return 0;
}

int fw_logic_deinit()
{
  if (&g_current_time != NULL)
  {
    fw_time_deinit(&g_current_time);
    LOG_DEBUG("g_current_time deinitialized");
  }
  return 0;
}

int main()
{
  int err = 0;

  err = fw_logic_init();
  if (err != 0)
  {
    return err;
  }

  while (1)
  {
    fw_driver_smbus_refresh();
    sleep_ms(50);
    tight_loop_contents();
  }

  return err;
}