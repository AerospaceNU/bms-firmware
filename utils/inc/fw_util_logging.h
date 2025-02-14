#ifndef FW_UTIL_LOGGING_H
#define FW_UTIL_LOGGING_H

//*************************************************************************************************
// Logging Module
//*************************************************************************************************

#define LOG_ERROR(a)            logError(FILENAME, __LINE__, a)
#define LOG_WARNING(a)          logWarning(FILENAME, __LINE__, a)
#define LOG_MESSAGE(a)          logMessage(FILENAME, __LINE__, a)
#define LOG_DEBUG(a)            logDebug(FILENAME, __LINE__, a)
#define LOG_INFO(a)             logError(FILENAME, __LINE__, a)



void logError(const char* file, const int line, const char* message);
void logWarning(const char* file, const int line, const char *message);
void logMessage(const char* file, const int line, const char *message);
void logDebug(const char* file, const int line, const char *message);
void logInfo(const char* file, const int line, const char *message);


//*************************************************************************************************

#endif // FW_UTIL_LOGGING_H