/* Global Includes */
#include <stdio.h>

/* Local Includes */
#include "pico/stdlib.h"

#include "utils/inc/fw_constants.h"
#include "utils/inc/fw_types.h"
#include "utils/inc/fw_util_logging.h"
#include "utils/inc/fw_globals.h"
#include "logic/inc/fw_logic_error.h"
#include "drivers/inc/fw_bq76972.h"
#include "logic/inc/fw_temperature.h"
#include "utils/inc/fw_time.h"


int fw_logic_init() {
  if (stdio_init_all() == false) {
    LOG_ERROR("Could not initialize stdio");
    return E_FW_LOGIC_INIT;
  }
  fw_time_init(g_current_time);
  LOG_DEBUG("g_current_time initialized");

  int err = fw_bq76972_spi_init();
  if (err != 1) {
    LOG_ERROR("Could not initialize BQ76972 SPI interface");
    return err;
  }
  LOG_DEBUG("BQ76972 SPI interface initialized");
  err = fw_therm_init();
  if (err != 1) {
    LOG_ERROR("Could not initialize temperature sensors");
    return err;
  }
}

int fw_logic_deinit() {
  if (g_current_time != NULL) {
    fw_time_deinit(g_current_time);
    LOG_DEBUG("g_current_time deinitialized");
  }
  fw_bq76972_spi_deinit();
  LOG_DEBUG("BQ76972 SPI interface deinitialized");
  fw_therm_deinit();
  LOG_DEBUG("Temperature sensors deinitialized");
  return 0;
}


int main() {
  int err = 0;

  err = fw_logic_init();

  // Main loop
  while(1) {
    LOG_MESSAGE("Temperature Sensor Readings:");
    float die_temp;
    float temperatures[6];
    err = fw_therm_read(FW_THERM_INTERNAL, &die_temp);
    if (err != 1) {
      LOG_ERROR("Failed to read die temperature");
    }
    err = fw_therm_read_all(temperatures);
    if (err != 1) {
      LOG_ERROR("Failed to read all thermistor temperatures");
    }
    char *buffer = malloc(50 * sizeof(char));
    sprintf(buffer, "Die Temperature: %.2f C", die_temp);
    LOG_MESSAGE(buffer);
    free(buffer);
    for (int i = 0; i < 6; i++) {
      buffer = malloc(50 * sizeof(char));
      sprintf(buffer, "Thermistor %d Temperature: %.2f C", i + 1, temperatures[i]);
      LOG_MESSAGE(buffer);
      free(buffer);
    }
    sleep_ms(2500); // Delay between readings
  }

  return err;
}