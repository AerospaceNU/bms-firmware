#ifndef FW_TYPES_H
#define FW_TYPES_H

#include <stdint.h>

// *************************************************************************************************
// System Types Module
// *************************************************************************************************

typedef struct fw_time {
    /* data */
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t millisecond;
} fw_time_t;



// *************************************************************************************************

#endif // FW_TYPES_H