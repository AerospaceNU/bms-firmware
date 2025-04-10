/* Global Includes */
#include <stdio.h>
#include <stdlib.h>

/* Local Includes */
#include "fw_util_logging.h"
#include "pico/stdlib.h"
#include "fw_time.h"
#include "fw_globals.h"


// What should a log message look like?
/* 
 * [Time since boot] [Log Type] [Origin File/Line]: [Message]
 * ex. 01:04:39:127 [I] (main.c/42):  Hello World!
*/

/// @brief Log a message to the Serial Monitor
/// @param file Origin File name
/// @param line Origin Line number
/// @param type Log Type
/// @param message Message
void fw_log(const char* file, const int line, const enum fw_log_type type, const char* message) {
  char* time = fw_time_to_string(fw_current_time(g_current_time)); // timestamp
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