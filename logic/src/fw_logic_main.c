/* Local Includes */
#include "pico/stdlib.h"
#include "fw_util_logging.h"

int main() {
  int err = 0;

  err = fw_logic_init();

  while(1) {
    logMessage(__FILE__, __LINE__, "Hello Message!");
    sleep_ms(2000);
    logDebug(__FILE__, __LINE__, "Hello Debug!!");
    sleep_ms(5000);
  }

  return err;
}

int fw_logic_init() {
  if (stdio_init_all() == false) {
    logError(__FILE__, __LINE__, "Could not initialize stdio");
  }
}