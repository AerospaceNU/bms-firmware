/* Local Includes */
#include "fw_util_logging.h"
#include "pico/stdlib.h"
#include "fw_time.h"



// What should a log message look like?
/* 
 * [Time since boot] [Log Type] [Origin File/Line]: [Message]
 * ex. 01:04:39:127 [I] (main.c/42):  Hello World!
*/

/// @brief Log a warning message to Serial Monitor.
/// @param file Origin File name
/// @param line Origin Line number
/// @param message Warning Message
void logWarning(const char* file, const int line, const char* message) {
  char* time = fw_time_to_string(fw_current_time()); // timestamp
  char* origin = sprintf("%s: %d", file, line);
  printf("%s [W] ( %s ): %s\n", time, origin, message);
}

/// @brief Log a message to the Serial Monitor
/// @param file Origin File name
/// @param line Origin Line number
/// @param message Message
void logMessage(const char* file, const int line, const char* message) {
  char* time = fw_time_to_string(fw_current_time()); // timestamp
  char* origin = sprintf("%s: %d", file, line);
  printf("%s [M] (origin goes here): %s\n", time, origin, message);
}

/// @brief Log a debug message to the Serial Monitor
/// @param file Origin File name
/// @param line Origin Line number
/// @param message Debug Message
void logDebug(const char* file, const int line, const char* message) {
  char* time = fw_time_to_string(fw_current_time()); // timestamp
  char* origin = sprintf("%s: %d", file, line);
  printf("%s [D] (origin goes here): %s\n", time, origin, message);
}

/// @brief Log an info message to the Serial Monitor
/// @param file Origin File name
/// @param line Origin Line number
/// @param message Info Message
void logInfo(const char* file, const int line, const char* message) {
  char* time = fw_time_to_string(fw_current_time()); // timestamp
  char* origin = sprintf("%s: %d", file, line);
  printf("%s [I] (origin goes here): %s\n", time, origin, message);
}