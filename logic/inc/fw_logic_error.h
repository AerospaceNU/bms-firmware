//*************************************************************************************************
//  Module                  :   fw_logic_error.h
//  Revision                :   0.1
//  Description             :   Custom error declarations and handler functions
//*************************************************************************************************
#ifndef _FW_LOGIC_ERROR_
#define _FW_LOGIC_ERROR_

#define E_SPI(x)        (100+x)

#define E_FW(x)         (1000 + x)
#define E_FW_LOGIC(x)   E_FW(100 + x)
#define E_PICO(x)       E_FW(200 + x)
#define E_BQ76972(x)    E_FW(300 + x)

/*************************************************************************************************/
/* Fixed Wing Logical Errors */
#define E_FW_LOGIC_INIT        E_FW_LOGIC(1)
#define E_FW_LOGIC_DEINIT      E_FW_LOGIC(2)

/*************************************************************************************************/
/* SPI Errors */
#define E_SPI_INIT      E_SPI(1)
#define E_SPI_WRITE     E_SPI(2)
#define E_SPI_READ      E_SPI(3)
#define E_SPI_WR        E_SPI(4)

/*************************************************************************************************/
/* Raspberry Pico System Errors */
#define E_PICO_IO       E_PICO(1)

/*************************************************************************************************/
/* BQ76972 Errors */
#define E_BQ76972_INIT              E_BQ76972(1)
#define E_BQ76972_READ              E_BQ76972(2)
#define E_BQ76972_WRITE             E_BQ76972(3)
#define E_BQ76972_OP_STATUS         E_BQ76972(4)

/*************************************************************************************************/
/* BQ76972 Driver Return Codes */

/** \brief Returned by BQ76972 driver functions on success. */
#define FW_BQ76972_SUCCESS 1

/**
 * \brief Constructs a BQ76972 SPI fault code from the device's SPI status fault index \p x.
 *
 * SPI fault codes occupy the range [100, 199]. The index \p x corresponds
 * directly to the fault reported in the device's SPI response byte.
 */
#define FW_BQ76972_SPI_ERR(x)                   (100 + (x))

/** \brief SPI error: unrecognised fault (fault index 0). */
#define FW_BQ76972_SPI_UNKNOWN_ERR              FW_BQ76972_SPI_ERR(0)
/** \brief SPI error: host read issued before device finished processing (fault index 1). */
#define FW_BQ76972_SPI_READ_TOO_FAST_ERR        FW_BQ76972_SPI_ERR(1)
/** \brief SPI error: internal oscillator is asleep; device not ready (fault index 2). */
#define FW_BQ76972_SPI_INTERNAL_OSC_ASLEEP_ERR  FW_BQ76972_SPI_ERR(2)
/** \brief SPI error: CRC in response does not match computed value (fault index 3). */
#define FW_BQ76972_SPI_CRC_MISMATCH_ERR         FW_BQ76972_SPI_ERR(3)
/** \brief SPI error: response opcode or length was not as expected (fault index 4). */
#define FW_BQ76972_SPI_UNEXPECTED_RESPONSE_ERR  FW_BQ76972_SPI_ERR(4)

#endif // _FW_LOGIC_ERROR_