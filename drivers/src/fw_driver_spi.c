/* Global Includes */
#include <stdio.h>
#include <stdlib.h>

/* Local Includes */
#include "fw_driver_spi.h"
#include "fw_constants.h"
#include "fw_util_logging.h"

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "fw_logic_error.h"

/// @brief Initialize the RaspberryPico SPI communication protocol.
/// @param spi_port spi0 or spi1 - the Pico port to use for SPI (check header for more info)
/// @param baudrate the SPI baudrate (typically 1MHz)
/// @param sdi "serial data in" GPIO pin number
/// @param csb "chip select (active low)" GPIO pin number
/// @param sck "serial clock" GPIO pin number
/// @param sdo "serial data out" GPIO pin number
/// @return error code (or 1 if successful)
int fw_spi_controller_init(spi_inst_t *spi_port, uint baudrate, uint cpol, uint cpha, uint order,
                           uint bits, uint sdi, uint csb, uint sck, uint sdo)
{
  LOG_INFO("Initializing SPI Controller");

  if (spi_init(spi_port, baudrate) != baudrate)
  {
    LOG_ERROR("SPI initialization failed");
    return E_SPI_INIT;
  }

  LOG_DEBUG("Setting SPI Format");
  spi_set_format(spi_port, bits, cpha, cpol, order);
  LOG_DEBUG("SPI format set successfully");

  LOG_DEBUG("Setting SPI pins");
  gpio_set_function(sdi, GPIO_FUNC_SPI);
  gpio_set_function(sck, GPIO_FUNC_SPI);
  gpio_set_function(sdo, GPIO_FUNC_SPI);

  LOG_DEBUG("Setting CSB pin as GPIO output");
  gpio_init(csb);
  gpio_set_dir(csb, GPIO_OUT);
  gpio_put(csb, 1);

  LOG_DEBUG("SPI pins set successfully");
  LOG_INFO("SPI initialized successfully");
  return 1;
}

/// @brief Deinitialize the RaspberryPico SPI communication protocol.
/// @param spi_port spi0 or spi1 - the Pico port to use for SPI (check header for more info)
void fw_spi_controller_deinit(spi_inst_t *spi_port, uint sdi, uint csb, uint sck, uint sdo)
{
  LOG_INFO("Deinitializing SPI Controller");
  spi_deinit(spi_port);

  LOG_DEBUG("Resetting SPI pins to GPIO safety state");
  gpio_set_function(sdi, GPIO_FUNC_SIO);
  gpio_set_function(sck, GPIO_FUNC_SIO);
  gpio_set_function(sdo, GPIO_FUNC_SIO);
  gpio_set_function(csb, GPIO_FUNC_SIO);

  LOG_DEBUG("Set SPI pins to inputs");
  gpio_set_dir(sdi, GPIO_IN);
  gpio_set_dir(sck, GPIO_IN);
  gpio_set_dir(sdo, GPIO_IN);
  gpio_set_dir(csb, GPIO_IN);

  LOG_INFO("SPI deinitialized successfully");
}

/// @brief Write and read data to/from the SPI device simultaneously.
/// @param spi_port spi0 or spi1 - the Pico port to use for SPI (check header for more info)
/// @param tx_buffer the buffer with data to write to the SPI device
/// @param rx_buffer the buffer to store data read from the SPI device
/// @param len the length of BOTH data buffers
/// @return error code (or 1 if successful)
int fw_spi_controller_wr_bl(spi_inst_t *spi_port, const uint8_t *tx_buffer, uint8_t *rx_buffer, size_t len, uint csb)
{
  // TODO: Implement SPI write read blocking
  LOG_INFO("Writing and reading data from SPI device");

  gpio_put(csb, 0); // Assert CSB (active low)
  if (spi_write_read_blocking(spi_port, tx_buffer, rx_buffer, len) != len)
  {
    LOG_ERROR("SPI write read failed");
    return E_SPI_WR;
  }
  gpio_put(csb, 1); // Deassert CSB
  LOG_INFO("Data written and read successfully");
  return 1;
}

int fw_spi_controller_r_bl(spi_inst_t *spi_port, uint8_t rep_tx_data, uint8_t *rx_buffer, size_t len, uint csb)
{
  // TODO: Implement SPI read blocking
  LOG_INFO("Reading data from SPI device");
  gpio_put(csb, 0); // Assert CSB (active low)
  if (spi_read_blocking(spi_port, rep_tx_data, rx_buffer, len) != len)
  {
    LOG_ERROR("SPI read failed");
    return E_SPI_READ;
  }
  gpio_put(csb, 1); // Deassert CSB
  LOG_INFO("Data read successfully");
  return 1;
}

int fw_spi_controller_w_bl(spi_inst_t *spi_port, const uint8_t *tx_buffer, size_t len, uint csb)
{
  // TODO: Implement SPI write blocking
  LOG_INFO("Writing data to SPI device");
  gpio_put(csb, 0); // Assert CSB (active low)
  if (spi_write_blocking(spi_port, tx_buffer, len) != len)
  {
    LOG_ERROR("SPI write failed");
    return E_SPI_WRITE;
  }
  gpio_put(csb, 1); // Deassert CSB
  LOG_INFO("Data written successfully");
  return 1;
}