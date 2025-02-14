/* Global Includes */
#include <stdio.h>
#include <stdlib.h>

/* Local Includes */
#include "fw_util_logging.h"
#include "pico/stdlib.h"
#include "fw_time.h"


// What should a log message look like?
/* 
 * [Time since boot] [Log Type] [Origin File/Line]: [Message]
 * ex. 01:04:39:127 [I] (main.c/42):  Hello World!
*/

/// @deprecated Call log instead, pass FW_LOG_ERROR as the type
/// @brief Log a error message to Serial Monitor.
/// @param file Origin File name
/// @param line Origin Line number
/// @param message Error Message
void logError(const char* file, const int line, const char* message) {
  char* time = fw_time_to_string(fw_current_time()); // timestamp
  printf("%s [E] (%s/%d): %s\n", time, file, line, message);
  free(time);
}

/// @deprecated Call log instead, pass FW_LOG_WARNING as the type
/// @brief Log a warning message to Serial Monitor.
/// @param file Origin File name
/// @param line Origin Line number
/// @param message Warning Message
void logWarning(const char* file, const int line, const char* message) {
  char* time = fw_time_to_string(fw_current_time()); // timestamp
  printf("%s [W] (%s/%d): %s\n", time, file, line, message);
  free(time);
}

/// @deprecated Call log instead, pass FW_LOG_MESSAGE as the type
/// @brief Log a message to the Serial Monitor
/// @param file Origin File name
/// @param line Origin Line number
/// @param message Message
void logMessage(const char* file, const int line, const char* message) {
  char* time = fw_time_to_string(fw_current_time()); // timestamp
  printf("%s [M] (%s/%d): %s\n", time, file, line, message);
  free(time);
}

/// @deprecated Call log instead, pass FW_LOG_DEBUG as the type
/// @brief Log a debug message to the Serial Monitor
/// @param file Origin File name
/// @param line Origin Line number
/// @param message Debug Message
void logDebug(const char* file, const int line, const char* message) {
  char* time = fw_time_to_string(fw_current_time()); // timestamp
  printf("%s [D] (%s/%d): %s\n", time, file, line, message);
  free(time);
}

/// @deprecated Call log instead, pass FW_LOG_INFO as the type
/// @brief Log an info message to the Serial Monitor
/// @param file Origin File name
/// @param line Origin Line number
/// @param message Info Message
void logInfo(const char* file, const int line, const char* message) {
  char* time = fw_time_to_string(fw_current_time()); // timestamp
  printf("%s [I] (%s/%d): %s\n", time, file, line, message);
  free(time);
}

/// @brief Log a message to the Serial Monitor
/// @param file Origin File name
/// @param line Origin Line number
/// @param type Log Type
/// @param message Message
void log(const char* file, const int line, const enum fw_log_type type, const char* message) {
  char* time = fw_time_to_string(fw_current_time()); // timestamp
  char type_ch;
  switch (type) {
    case FW_LOG_ERROR:
      type_ch = 'E';
      break;
    case FW_LOG_WARNING:
      type_ch = 'W';
      break;
    case FW_LOG_MESSAGE:
      type_ch = 'M';
      break;
    case FW_LOG_DEBUG:
      type_ch = 'D';
      break;
    case FW_LOG_INFO:
      type_ch = 'I';
      break;
    default:
      // Log an error, unknown log type.
      LOG_ERROR("Unknown log type");
      break;
  }
  printf("%s [%c] (%s/%d): %s\n", time, type, file, line, message);
  free(time);
}