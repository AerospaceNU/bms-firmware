/* Global Includes */
#include <stdio.h>

/* Local Includes */
#include "drivers/inc/fw_driver_spi.h"
#include "utils/inc/fw_constants.h"
#include "utils/inc/fw_types.h"

#include "pico/stdlib.h"
#include "fw_util_logging.h"

int fw_logic_init() {
  if (stdio_init_all() == false) {
    LOG_ERROR("Could not initialize stdio");
  }
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