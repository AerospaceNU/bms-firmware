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

  int ec = fw_spi_controller_init(spi0, MEGAHZ, PIN_SDI, PIN_CSB, PIN_SCK, PIN_SDO);

  return ec;
}

int main() {
  int err = 0;

  err = fw_logic_init();

  sleep_ms(5000);
  LOG_MESSAGE("Hello Message!");

  uint8_t spi_in_buf[SPI_MAX_INPUT_SIZE];

  while(1) {
    fw_spi_controller_r_bl(spi0, NULL, spi_in_buf, SPI_MAX_INPUT_SIZE);
    LOG_MESSAGE("SPI IN:");
    for (int i = 0; i < SPI_MAX_INPUT_SIZE; i++) {
      printf("%02X ", spi_in_buf[i]);
    }
  }

  return err;
}