#include "fw_time.h"
#include "fw_globals.h"
#include "fw_constants.h"

#include "pico/time.h"

/// @deprecated We dont need an init function for fw_time as the timer is already started
///   upon boot.
void fw_time_init() {
  fw_time_t fw_time_since_boot;
  uint64_t ms = time_us_64() / 1000;
  fw_start_time.hour = ms / 3600000;
  ms %= 3600000;
  fw_start_time.minute = ms / 60000;
  ms %= 60000;
  fw_start_time.second = ms / 1000;
  fw_start_time.millisecond = ms % 1000;
}

/// @brief Return the current time when called.
/// @return Current time
fw_time_t fw_current_time() {
  fw_time_t fw_ct; // Store the current time.
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
static char *fw_time_to_string(fw_time_t time) {
  char *fw_time_str = (char *)malloc(sizeof(char *) * TIMESTAMP_SIZE);
  sprintf(fw_time_str, "%ld:%ld:%ld:%ld", time.hour, time.minute, time.second, time.minute);
  return fw_time_str;
}