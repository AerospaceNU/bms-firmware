/* Global Includes */
#include <stdio.h>

/* Local Includes */
#include "pico/stdlib.h"

#include "utils/inc/fw_constants.h"
#include "utils/inc/fw_types.h"
#include "utils/inc/fw_util_logging.h"
#include "utils/inc/fw_globals.h"
#include "logic/inc/fw_logic_error.h"
#include "drivers/inc/fw_driver_smbus.h"

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
    tight_loop_contents();
  }

  return err;
}