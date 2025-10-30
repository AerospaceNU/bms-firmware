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

#endif // _FW_LOGIC_ERROR_