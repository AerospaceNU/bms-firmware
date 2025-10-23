#ifndef FW_TYPES_H
#define FW_TYPES_H

#include <stdint.h>
#include <string.h>

// *************************************************************************************************
// System Types Module
// *************************************************************************************************

#define FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

typedef struct fw_time {
    /* data */
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t millisecond;
} fw_time_t;

enum fw_log_type {
    FW_LOG_ERROR,
    FW_LOG_WARNING,
    FW_LOG_MESSAGE,
    FW_LOG_DEBUG,
    FW_LOG_INFO
};


// *************************************************************************************************

#endif // FW_TYPES_H