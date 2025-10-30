//*************************************************************************************************
//  Module                  :   fw_logic_error.h
//  Revision                :   0.1
//  Description             :   Custom error declarations and handler functions
//*************************************************************************************************
#ifndef _FW_TEMPERATURE_H_
#define _FW_TEMPERATURE_H_

/*************************************************************************************************/
/* Thermistor Command Addresses */
#define INTERNAL_ADDR      0x68 // Internal Die Temperature
#define CFETOFF_ADDR       0x6A // CFETOFF Pin Thermistor Address
#define DFETOFF_ADDR       0x6C // DFETOFF Pin Thermistor Address
#define ALERT_ADDR         0x6E // ALERT Pin Thermistor Address
#define TS1_ADDR           0x70 // TS1 Pin Thermistor Address
#define TS2_ADDR           0x72 // TS2 Pin Thermistor Address
#define TS3_ADDR           0x74 // TS3 Pin Thermistor Address
#define HDQ_ADDR           0x76 // HDQ Pin Thermistor Address
#define DCHG_ADDR          0x78 // DCHG Pin Thermistor Address
#define DDSG_ADDR          0x7A // DDSG Pin Thermistor Address


#endif // _FW_TEMPERATURE_H_