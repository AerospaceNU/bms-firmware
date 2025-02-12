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


// What should a log message look like?
/* 
 * [Time since boot] [Origin File/Line] [Log Type] [Message]
 * ex. 01:04:39:127 (main.c/42) [I]:  Hello World!
*/


void logWarning(const char *message);

void logMessage(const char *message);

void logDebug(const char *message);

void logInfo(const char *message);


//*************************************************************************************************

#endif // FW_UTIL_LOGGING_H