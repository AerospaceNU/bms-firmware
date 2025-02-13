#ifndef FW_UTIL_LOGGING_H
#define FW_UTIL_LOGGING_H

//*************************************************************************************************
// Logging Module
//*************************************************************************************************



// Log Level (optional)

// log warning
// log message
// log debug
// log info





void logWarning(const char* file, const int line, const char *message);

void logMessage(const char* file, const int line, const char *message);

void logDebug(const char* file, const int line, const char *message);

void logInfo(const char* file, const int line, const char *message);


//*************************************************************************************************

#endif // FW_UTIL_LOGGING_H