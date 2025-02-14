#ifndef FW_UTIL_LOGGING_H
#define FW_UTIL_LOGGING_H

#include "fw_types.h"
//*************************************************************************************************
// Logging Module
//*************************************************************************************************

#define LOG_ERROR(a)            log(FILENAME, __LINE__, FW_LOG_ERROR, a)
#define LOG_WARNING(a)          log(FILENAME, __LINE__, FW_LOG_WARNING, a)
#define LOG_MESSAGE(a)          log(FILENAME, __LINE__, FW_LOG_MESSAGE, a)
#define LOG_DEBUG(a)            log(FILENAME, __LINE__, FW_LOG_DEBUG, a)
#define LOG_INFO(a)             log(FILENAME, __LINE__, FW_LOG_ERROR, a)


//*************************************************************************************************

#endif // FW_UTIL_LOGGING_H