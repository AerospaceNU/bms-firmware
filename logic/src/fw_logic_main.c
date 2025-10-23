/* Global Includes */
#include <stdio.h>

/* Local Includes */
#include "pico/stdlib.h"

#include "utils/inc/fw_constants.h"
#include "utils/inc/fw_types.h"
#include "utils/inc/fw_util_logging.h"
#include "utils/inc/fw_globals.h"
#include "logic/inc/fw_logic_error.h"


int fw_logic_init() {
  if (stdio_init_all() == false) {
    LOG_ERROR("Could not initialize stdio");
    return E_FW_LOGIC_INIT;
  }
  fw_time_init(g_current_time);
  LOG_DEBUG("g_current_time initialized");
}

int fw_logic_deinit() {
  if (g_current_time != NULL) {
    fw_time_deinit(g_current_time);
    LOG_DEBUG("g_current_time deinitialized");
  }
  return 0;
}


int main() {
  int err = 0;

  err = fw_logic_init();

  while(1) {
    LOG_MESSAGE("Hello Message!");
    sleep_ms(2000);
    LOG_DEBUG("Hello Debug!!");
    sleep_ms(5000);
  }

  return err;
}