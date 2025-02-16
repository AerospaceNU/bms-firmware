#ifndef FW_UTIL_LOGGING_H
#define FW_UTIL_LOGGING_H

#include "fw_types.h"
//*************************************************************************************************
// Logging Module
//*************************************************************************************************

#define LOG_ERROR(a)            fw_log(FILENAME, __LINE__, FW_LOG_ERROR, a)
#define LOG_WARNING(a)          fw_log(FILENAME, __LINE__, FW_LOG_WARNING, a)
#define LOG_MESSAGE(a)          fw_log(FILENAME, __LINE__, FW_LOG_MESSAGE, a)
#define LOG_DEBUG(a)            fw_log(FILENAME, __LINE__, FW_LOG_DEBUG, a)
#define LOG_INFO(a)             fw_log(FILENAME, __LINE__, FW_LOG_ERROR, a)

void logError(const char* file, const int line, const char* message);
void logWarning(const char* file, const int line, const char* message);
void logMessage(const char* file, const int line, const char* message);
void logDebug(const char* file, const int line, const char* message);
void logInfo(const char* file, const int line, const char* message);

void fw_log(const char* file, const int line, const enum fw_log_type type, const char* message);


//*************************************************************************************************

#endif // FW_UTIL_LOGGING_H