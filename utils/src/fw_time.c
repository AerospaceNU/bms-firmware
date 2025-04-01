/* Global Includes */
#include <stdlib.h>
#include <stdio.h>

/* Local Includes */
#include "fw_time.h"
#include "fw_globals.h"
#include "fw_constants.h"
#include "fw_util_logging.h"

#include "pico/time.h"

/// @brief Global variable to store the current time.
void fw_time_init(fw_time_t *ptr) {
  if (ptr == NULL) {
    ptr = (fw_time_t *)malloc(sizeof(fw_time_t));
  } else {
    LOG_WARNING("g_current_time already initialized");
  }
  assert(ptr != NULL);
  LOG_DEBUG("g_current_time initialized");
  return;
}
/// @brief Deinitialize the time module.
void fw_time_deinit(fw_time_t *ptr) {
  if (ptr != NULL) {
    free(ptr);
    ptr = NULL;
  } else {
    LOG_WARNING("g_current_time already deinitialized");
  }
  assert(ptr == NULL);
  LOG_DEBUG("g_current_time deinitialized");
  return;
}

/// @brief Return the current time when called.
/// @param fw_ct The current time structure.
/// @note fw_time_init() must be called before this function.
/// @return Current time
fw_time_t fw_current_time(fw_time_t fw_ct) {
  uint64_t ms = time_us_64() / 1000;
  fw_ct.hour = ms / 3600000;
  ms %= 3600000;
  fw_ct.minute = ms / 60000;
  ms %= 60000;
  fw_ct.second = ms / 1000;
  fw_ct.millisecond = ms % 1000;

  return fw_ct;
}


/// @brief Convert a fw_time_t to a String
/// @return String timestamp
char* fw_time_to_string(fw_time_t time) {
  char *fw_time_str = (char *)malloc(sizeof(char *) * TIMESTAMP_SIZE);
  sprintf(fw_time_str, "%02ld:%02ld:%02ld:%03ld", time.hour, time.minute, time.second, time.millisecond);
  return fw_time_str;
}