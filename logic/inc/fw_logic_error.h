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

/*************************************************************************************************/
/* Fixed Wing Logical Errors */
#define E_FW_LOGIC_INIT        E_FW_LOGIC(1)
#define E_FW_LOGIC_DEINIT      E_FW_LOGIC(2)

/*************************************************************************************************/
/* SPI Errors */
#define E_SPI_INIT      E_SPI(1)

/*************************************************************************************************/
/* Raspberry Pico System Errors */
#define E_PICO_IO       E_PICO(1)

#endif // _FW_LOGIC_ERROR_