// Global Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Local Includes
#include "fw_bq76972.h"
#include "fw_driver_spi.h"
#include "fw_util_logging.h"
#include "fw_logic_error.h"
#include "fw_constants.h"


/**
 * @brief Initialize BQ76972 SPI Interface
 * @param spi_port spi0 or spi1 - the Pico port to use for SPI (check header for more info)
 * @return error code (or 1 if successful)
 */
int fw_bq76972_spi_init() {
    LOG_INFO("Initializing BQ76972 SPI Interface...");
    uint err = fw_spi_controller_init(SPI_PORT, BAUD_RATE, POLARITY, PHASE, ORDER, BITS_PER_WORD, PIN_SDI, PIN_CSB, PIN_SCK, PIN_SDO);
    if (err != 1) {
        LOG_ERROR("Failed to initialize BQ76972 SPI Interface");
        return err;
    }
    LOG_INFO("BQ76972 SPI Interface initialized successfully");
    return 1;
}

/**
 * @brief Deinitialize BQ76972 SPI Interface
 * @param spi_port spi0 or spi1 - the Pico port to use for SPI (check header for more info)
 * @param sdi "serial data in" GPIO pin number
 * @param csb "chip select (active low)" GPIO pin number
 * @param sck "serial clock" GPIO pin number
 * @param sdo "serial data out" GPIO pin number
 * @return error code (or 1 if successful)
 */
void fw_bq76972_spi_deinit() {
    LOG_INFO("Deinitializing BQ76972 SPI Interface...");
    fw_spi_controller_deinit(SPI_PORT, PIN_SDI, PIN_CSB, PIN_SCK, PIN_SDO);
    LOG_INFO("BQ76972 SPI Interface deinitialized successfully");
}

/**
 * @brief Write Data to BQ76972 Data Memory Address
 * @param mem_addr Memory address to write data
 * @param data Data to write to memory address
 * @param len Length of data in bytes
 * @return error code (or 1 if successful)
 */
int fw_bq76972_write_data_mem(uint16_t mem_addr, uint8_t *data, size_t len) {
    /* Steps
     * Write LOWER_BYTE register with lower byte of addr
     * Write UPPER_BYTE register with upper byte of addr
     * Write data to transfer buffer register (0x40 to 0x5F) in little endian format
     * Write checksum of data written to 0x60
     * Write length of data to 0x61
     * Verify data by reading back from memory address
    */
    
    // Split mem_addr into lower and upper bytes
    uint8_t address[2] = { (uint8_t)(mem_addr & 0xFF), (uint8_t)((mem_addr >> 8) & 0xFF) };
    // Write LOWER_BYTE and UPPER_BYTE registers
    int err = fw_bq76971_write_register(REG_ADDRESS, address, 2);
    if (err != 1) {
        LOG_ERROR("Failed to write memory address to BQ76972");
        return err;
    }

    // Prepare data to write in little endian format
    uint8_t data_buffer[] = &data;

    // Write data to transfer buffer
    err = fw_bq76971_write_register(TRANSFER_BUFFER, data_buffer, len);
    if (err != 1) {
        LOG_ERROR("Failed to write data to BQ76972 transfer buffer");
        return err;
    }
    // Calculate checksum
    size_t integrity_len = len + 2 + 2; // +2 for address bytes, +2 for integrity bytes
    uint8_t checksum = _calculate_checksum(mem_addr, integrity_len);
    // Write checksum and length to INTEGRITY_ADDRESS
    uint8_t integrity[2] = { checksum, integrity_len};
    err = fw_bq76971_write_register(INTEGRITY_ADDRESS, integrity, 2);
    if (err != 1) {
        LOG_ERROR("Failed to write integrity data to BQ76972");
        return err;
    }

    // Verify data by reading back from memory address
    uint16_t verify_data;
    err = fw_bq76972_read_data_subcommand(mem_addr, &verify_data);
    if (err != 1) {
        LOG_ERROR("Failed to verify data written to BQ76972");
        return err;
    }
    if (verify_data != data) {
        LOG_ERROR("Data verification mismatch for BQ76972 write");
        return E_BQ76972_WRITE;
    }
}

/**
 * @brief Read Data from BQ76972 Subcommand
 * @param mem_addr Memory address to read data from
 * @param data Pointer to store data read from memory address
 * @return error code (or 1 if successful)
 */
int fw_bq76972_read_data_subcommand(uint16_t mem_addr, uint16_t *data) {
    /* Steps
     * Write LOWER_BYTE register with lower byte of addr
     * Write UPPER_BYTE register with upper byte of addr
     * Read register 0x3E and 0x3F to get operation status
     * if both registers return 0xFF, then operation has not completed,
     * retry until registers read what was written originally
     * Read length of response from 0x61
     * Read buffer from 0x40 to 0x5F for expected length
     * Read checksum from 0x60 and verify data integrity
    */
   
    // Split mem_addr into lower and upper bytes
    uint8_t address[2] = { (uint8_t)((mem_addr & 0xFF) | WRITE_BIT), (uint8_t)(((mem_addr >> 8) & 0xFF) | WRITE_BIT) };

    // Write LOWER_BYTE and UPPER_BYTE registers
    int err = fw_bq76971_write_register(REG_ADDRESS, address, 2);
    if (err != 1) {
        LOG_ERROR("Failed to write memory address to BQ76972");
        return err;
    }

    // Read operation status from 0x3E and 0x3F
    uint8_t op_status[2];
    do {
        int err = fw_bq76971_read_register(REG_ADDRESS, op_status, 2);
        if (err != 1) {
            LOG_ERROR("Failed to read operation status from BQ76972");
            return err;
        }
    } while (op_status[0] == 0xFF && op_status[1] == 0xFF);

    if (op_status[0] != address[LOWER_BYTE] && op_status[UPPER_BYTE] == address[1]) {
            LOG_ERROR("Failed to read operation status from BQ76972");
            return E_BQ76972_OP_STATUS;
    }

    uint8_t integrity[2];
    // Read checksum and data length from 0x60
    int err = fw_bq76971_read_register(INTEGRITY_ADDRESS, integrity, 2);
    if (err != 1) {
        LOG_ERROR("Failed to read integrity data from BQ76972");
        return err;
    }
    size_t data_len = integrity[LENGTH]-2-2; // -2 for address bytes, -2 for integrity bytes
    uint8_t transfer_buffer[data_len];
    // Read data from transfer buffer
    err = fw_bq76971_read_register(TRANSFER_BUFFER, transfer_buffer, data_len);
    if (err != 1) {
        LOG_ERROR("Failed to read data from BQ76972 transfer buffer");
        return err;
    }
    // Calculate checksum
    
    uint8_t checksum_calc = _calculate_checksum(mem_addr, integrity[LENGTH]);

    if (checksum_calc != integrity[CHECKSUM]) {
        LOG_ERROR("Checksum verification failed for BQ76972 data read");
        return E_BQ76972_READ;
    }
    // Copy data to output parameter
    memcpy(data, transfer_buffer, integrity[LENGTH]);
    return 1;
}

/**
 * @brief Read SPI Register from BQ76972
 * @param reg_addr Register address to read from
 * @param data Pointer to store data read from register
 * @param len Length of data to read
 * @return error code (or 1 if successful)
 */
int fw_bq76971_read_register(uint8_t reg_addr, uint8_t *data, size_t len) {
    uint8_t tx_buffer[len + 1];
    memset(tx_buffer, 0x00, sizeof tx_buffer); // Initialize entire buffer to 0x00
    tx_buffer[0] = (uint8_t) (reg_addr & READ_BIT); // Set the first byte to the register address
    uint8_t rx_buffer[len + 1];

    if (fw_spi_controller_wr_bl(SPI_PORT, tx_buffer, rx_buffer, len + 1, PIN_CSB) != 1) {
        LOG_ERROR("Failed to read register from BQ76972");
        return E_BQ76972_READ;
    }

    memcpy(&rx_buffer[1], data, len); // Copy received data excluding the first byte (register address)
    return 1;
}

/**
 * @brief Write SPI Register to BQ76972
 * @param reg_addr Register address to write to
 * @param data Pointer to data to write to register
 * @param len Length of data to write
 * @return error code (or 1 if successful)
 */
int fw_bq76971_write_register(uint8_t reg_addr, uint8_t *data, size_t len) {
    uint8_t tx_buffer[len + 1];
    tx_buffer[0] = (uint8_t)(reg_addr | WRITE_BIT); // Set the first byte to the register address
    memcpy(&tx_buffer[1], data, len); // Copy data to be written after the register address

    if (fw_spi_controller_w_bl(SPI_PORT, tx_buffer, len + 1, PIN_CSB) != 1) {
        LOG_ERROR("Failed to write register to BQ76972");
        return E_BQ76972_WRITE;
    }

    return 1;
}

/**
 * @brief Calculate checksum for BQ76972 data transfer
 * @param mem_addr Memory address involved in the transfer
 * @param len Length of data being transferred
 * @return Calculated checksum byte
 */
static uint8_t _calculate_checksum(uint16_t mem_addr, size_t len) {
    uint8_t checksum = 0;
    // Add lower and upper bytes of memory address
    checksum += (uint8_t)(mem_addr & 0xFF);
    checksum += (uint8_t)((mem_addr >> 8) & 0xFF);
    // Add length byte
    checksum += (uint8_t)(len & 0xFF);
    // Return bitwise inverted checksum
    return ~checksum;
}