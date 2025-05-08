/* Global Includes */
#include <stdio.h>

/* Local Includes */
#include "fw_driver_spi.h"
#include "fw_constants.h"
#include "fw_logic_error.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"

// Testing the spi_wrapper class between two pico boards
// The first pico board will be the master and the second pico board will be the slave
// The master will send a byte to the slave and the slave will send a byte back to the master
// The master will then compare the byte received from the slave with the byte sent to the slave

uint8_t master_buffer[2] = {0x01, 0x02};
uint8_t slave_buffer[2] = {0x00, 0x00};
uint8_t master_buffer_size = 2;
uint8_t slave_buffer_size = 2;

if (fw_spi_controller_init(spi0, 1000000, 0, 1, 2, 3) != 1) {
    printf("SPI controller init failed\n");
    return -1;
}
else {
    printf("SPI controller init successful.\n");
    printf("Master SPI controller initialized.\n");
}

if (fw_spi_controller_init(spi1, 1000000, 4, 5, 6, 7) != 1) {
    printf("SPI controller init failed\n");
    return -1;
}
else {
    printf("SPI controller init successful.\n");
    printf("Slave SPI controller initialized.\n");
}

// test a write and read to the SPI bus
// the master will send a byte to the slave and the slave will send a byte back to the master
// the master will then compare the byte received from the slave with the byte sent to the slave

if (fw_spi_write(spi0, master_buffer, master_buffer_size) != 1) {
    printf("SPI write failed\n");
    return -1;
}
else {
    printf("SPI write successful.\n");
    printf("Master wrote: %x %x\n", master_buffer[0], master_buffer[1]);
}

if (fw_spi_read(spi1, slave_buffer, slave_buffer_size) != 1) {
    printf("SPI read failed\n");
    return -1;
}
else {
    printf("SPI read successful.\n");
    printf("Slave read: %x %x\n", slave_buffer[0], slave_buffer[1]);
}