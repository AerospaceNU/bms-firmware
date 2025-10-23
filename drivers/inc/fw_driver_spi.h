#ifndef FW_DRIVER_SPI_H
#define FW_DRIVER_SPI_H

#include "hardware/spi.h"

/*************************************************************************************************/
/* SPI port defines */
// the following pins support SPI port 0 (spi0)
// GPIO0    (RX/SDI)
// GPIO1    (CSn/CSB)
// GPIO2    (SCK/SCK)
// GPIO3    (TX/SDO)

// GPIO4    (RX/SDI)
// GPIO5    (CSn/CSB)
// GPIO6    (SCK/SCK)
// GPIO7    (TX/SDO)

// *GPIO16  (RX/SDI)
// *GPIO17  (CSn/CSB)
// *GPIO18  (SCK/SCK)
// *GPIO19  (TX/SDO)

// * = default
/*************************************************************************************************/
#define SPI_PORT spi0

int fw_spi_controller_init(spi_inst_t *spi_port, uint baudrate,
  uint sdi, uint csb, uint sck, uint sdo);
void fw_spi_controller_deinit(spi_inst_t *spi_port);
int fw_spi_controller_wr_bl(spi_inst_t *spi_port, const uint8_t *tx_buffer, uint8_t *rx_buffer, size_t len);
int fw_spi_controller_r_bl(spi_inst_t *spi_port, const uint8_t *rep_tx_buffer, uint8_t *rx_buffer, size_t len);
int fw_spi_controller_w_bl(spi_inst_t *spi_port, const uint8_t *tx_buffer, size_t len);


#endif // FW_DRIVER_SPI_H