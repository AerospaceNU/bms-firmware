#ifndef FW_BQ76972_H
#define FW_BQ76972_H

/*************************************************************************************************/
/* BQ76972 Driver */
/*************************************************************************************************/

#include <stdint.h>

// BQ76972 Register Addresses
#define REG_ADDRESS         0x3E                // Register address for subcommands (16-bit address)
#define TRANSFER_BUFFER     0x40                // Transfer buffer address (max 32 bytes)
#define INTEGRITY_ADDRESS   0x60                // Checksum register address (2 bytes)

// BQ76972 Register Definitions
#define LOWER_BYTE          0                   // First byte of REG_ADDRESS
#define UPPER_BYTE          1                   // Second byte of REG_ADDRESS
#define CHECKSUM            0                   // First byte of INTEGRITY_ADDRESS
#define LENGTH              1                   // Second byte of INTEGRITY_ADDRESS

// BQ76972 Operation Bits
#define WRITE_BIT           0b10000000          // Bit to indicate write operation (bitwise OR with address)
#define READ_BIT            0b01111111          // Bit to indicate read operation (bitwise AND with address)

// BQ76972 Specifications
#define BAUD_RATE           1000*1000           // Maximum SPI Baud Rate for BQ76972 (1 MHz)
#define BITS_PER_WORD       8                   // Bits per word for SPI communication
#define POLARITY            0                   // SPI Clock Polarity
#define PHASE               0                   // SPI Clock Phase
#define ORDER               SPI_MSB_FIRST       // SPI Data Order


int fw_bq76972_spi_init();
void fw_bq76972_spi_deinit();
int fw_bq76972_write_data_subcommand(uint16_t mem_addr, uint16_t data);
int fw_bq76972_read_data_subcommand(uint16_t mem_addr, uint16_t *data);
int fw_bq76971_read_register(uint8_t reg_addr, uint8_t *data, size_t len);
int fw_bq76971_write_register(uint8_t reg_addr, uint8_t *data, size_t len);

/*************************************************************************************************/


#endif // FW_BQ76972_H