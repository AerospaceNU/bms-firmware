/* Local Includes */
#include "fw_util_logging.h"
#include "pico/stdlib.h"
#include "fw_time.h"

/// @brief Log a warning message to Serial Monitor.
/// @param message Warning Message
void logWarning(const char *message) {
  char *time = fw_current_time();
  
  printf("%s () [W]: %s\n",time, message);
}

/// @brief Log a message to the Serial Monitor
/// @param message Message
void logMessage(const char *message) {
  printf("[M]: %s\n", message);
}

/// @brief Log a debug message to the Serial Monitor
/// @param message Debug Message
void logDebug(const char *message) {
  printf("[D]: %s\n", message);
}

/// @brief Log an info message to the Serial Monitor
/// @param message Info Message
void logInfo(const char *message) {
  printf("[I]: %s\n", message);
}