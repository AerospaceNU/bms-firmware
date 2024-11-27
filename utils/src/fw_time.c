#include "fw_time.h"
#include "fw_globals.h"

#include "pico/time.h"

void fw_time_init() {
    uint32_t ms = to_ms_since_boot(get_absolute_time());
    fw_start_time.hour = ms / 3600000;
    ms %= 3600000;
    fw_start_time.minute = ms / 60000;
    ms %= 60000;
    fw_start_time.second = ms / 1000;
    fw_start_time.millisecond = ms % 1000;
}