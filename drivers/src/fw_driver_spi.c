/* Global Includes */
#include <stdio.h>

/* Local Includes */
#include "fw_driver_spi.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "logic/inc/fw_logic_error.h"

/// @brief Initialize the RaspberryPico SPI communication protocol.
/// @param spi_port spi0 or spi1 - the Pico port to use for SPI (check header for more info)
/// @param baudrate the SPI baudrate (typically 1MHz)
/// @param sdi "serial data in" GPIO pin number
/// @param csb "chip select (active low)" GPIO pin number
/// @param sck "serial clock" GPIO pin number
/// @param sdo "serial data out" GPIO pin number
/// @return error code (or 1 if successful)
int fw_spi_controller_init(spi_inst_t *spi_port, uint baudrate,
  uint sdi, uint csb, uint sck, uint sdo) {
  if (spi_init(spi_port, baudrate) != baudrate) {
    // TODO: handler?
    return E_SPI_INIT;
  }

  gpio_set_function(sdi, GPIO_FUNC_SPI);
  gpio_set_function(csb, GPIO_FUNC_SIO);
  gpio_set_function(sck, GPIO_FUNC_SPI);
  gpio_set_function(sdo, GPIO_FUNC_SPI);

  gpio_set_dir(csb, GPIO_OUT);
  gpio_put(csb, 1);

  return 1;
}


int fw_spi_controller() {
  char *buffer = calloc(256, sizeof (char));
    
  while(fgets(buffer, 256, stdin) != NULL) {

  }
}