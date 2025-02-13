/* Local Includes */
#include "fw_util_logging.h"
#include "pico/stdlib.h"
#include "fw_time.h"



// What should a log message look like?
/* 
 * [Time since boot] [Origin File/Line] [Log Type] [Message]
 * ex. 01:04:39:127 (main.c/42) [I]:  Hello World!
*/



/// @brief Log a warning message to Serial Monitor.
/// @param message Warning Message
void logWarning(const char *message) {
  printf("%s [W] (origin goes here): %s\n",fw_time_to_string(fw_current_time()), message);
}

/// @brief Log a message to the Serial Monitor
/// @param message Message
void logMessage(const char *message) {
  printf("%s [M] (origin goes here): %s\n",fw_time_to_string(fw_current_time()), message);
}

/// @brief Log a debug message to the Serial Monitor
/// @param message Debug Message
void logDebug(const char *message) {
  printf("%s [D] (origin goes here: %s\n",fw_time_to_string(fw_current_time()), message);
}

/// @brief Log an info message to the Serial Monitor
/// @param message Info Message
void logInfo(const char *message) {
  printf("%s [I] (origin goes here: %s\n",fw_time_to_string(fw_current_time()), message);
}