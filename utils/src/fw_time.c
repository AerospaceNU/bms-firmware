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
void fw_time_init(fw_time_t *ptr)
{
  if (ptr == NULL)
  {
    LOG_ERROR("fw_time_init called with NULL pointer");
    return;
  }

  ptr->hour = 0;
  ptr->minute = 0;
  ptr->second = 0;
  ptr->millisecond = 0;

  LOG_DEBUG("g_current_time initialized");
  return;
}
/// @brief Deinitialize the time module.
void fw_time_deinit(fw_time_t *ptr)
{
  if (ptr == NULL)
  {
    LOG_WARNING("g_current_time already deinitialized");
    return;
  }

  ptr->hour = 0;
  ptr->minute = 0;
  ptr->second = 0;
  ptr->millisecond = 0;

  LOG_DEBUG("g_current_time deinitialized");
  return;
}

/// @brief Return the current time when called.
/// @param fw_ct The current time structure.
/// @note fw_time_init() must be called before this function.
/// @return Current time
fw_time_t fw_current_time(fw_time_t fw_ct)
{
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
char *fw_time_to_string(fw_time_t time)
{
  char *fw_time_str = (char *)malloc(TIMESTAMP_SIZE);
  if (fw_time_str == NULL)
  {
    return NULL;
  }

  snprintf(fw_time_str,
           TIMESTAMP_SIZE,
           "%02u:%02u:%02u:%03u",
           (unsigned)time.hour,
           (unsigned)time.minute,
           (unsigned)time.second,
           (unsigned)time.millisecond);
  return fw_time_str;
}