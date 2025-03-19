//*************************************************************************************************
//  Module                  :   fw_logic_error.h
//  Revision                :   0.1
//  Description             :   Custom error declarations and handler functions
//*************************************************************************************************
#ifndef _FW_LOGIC_ERROR_
#define _FW_LOGIC_ERROR_

#define E_SPI(x)        (100+x)

#define E_FW(x)         (1000 + x)
#define E_PICO(x)       E_FW(100 + x)

/*************************************************************************************************/
/* SPI Errors */
#define E_SPI_INIT      E_SPI(1)
#define E_SPI_WRITE     E_SPI(2)
#define E_SPI_READ      E_SPI(3)

/*************************************************************************************************/
/* Raspberry Pico System Errors */
#define E_PICO_IO       E_PICO(1)

#endif // _FW_LOGIC_ERROR_