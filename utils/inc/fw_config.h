#ifndef FW_CONFIG_H
#define FW_CONFIG_H

//*************************************************************************************************
// Configuration Defines
//*************************************************************************************************


//*************************************************************************************************
// Thermistor Configuration
//*************************************************************************************************

/* Thermistor Addresses */
#define FW_THERM_INTERNAL       0x68            // Internal Die Temperature
#define FW_THERM_1              0x70            // TS1
#define FW_THERM_2              0x72            // TS2
#define FW_THERM_3              0x74            // TS3
#define FW_THERM_4              0x6C            // DFETOFF
#define FW_THERM_5              0x78            // DCHG
#define FW_THERM_6              0x7A            // DDSG

/* Thermistor FXN Bits */
#define FW_FXN_THERM            0b11            // Bitwise OR to enable thermistor readings

/* Thermistor Option Bits */
// Pullup Control
#define FW_18K_PULLUP           0b00 << 4       // 18k Ohm Pullup
#define FW_180K_PULLUP          0b01 << 4       // 180k Ohm Pullup
#define FW_NO_PULLUP            0b10 << 4       // No Pullup (for ADCIN)

// Temperature Measurement Model Bits
#define FW_18K_MODEL            0b00 << 2       // 18k Ohm Thermistor Model
#define FW_180K_MODEL           0b01 << 2       // 180k Ohm Thermistor Model
#define FW_CUSTOM_MODEL         0b10 << 2       // Custom Thermistor Model
#define FW_NO_MODEL             0b11 << 2       // No Thermistor Model, Raw ADCIN reported

// Measurement Type
#define FW_GEN_ADCIN            0b00            // General ADCIN Measurement
#define FW_CELL_TEMP_PROT       0b01            // Cell Temperature Protection Measurement
#define FW_CELL_TEMP            0b10            // Cell Temperature Measurement (not for protection)
#define FW_FET_TEMP_PROT        0b11            // FET Temperature Measurement


//*************************************************************************************************

#endif // FW_CONFIG_H
