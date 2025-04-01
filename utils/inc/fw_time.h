#ifndef FW_TIME_H
#define FW_TIME_H

//*************************************************************************************************
// System Time Module
//*************************************************************************************************

#include "fw_types.h"

void fw_time_init(fw_time_t *ptr);
void fw_time_deinit(fw_time_t *ptr);
fw_time_t fw_current_time(fw_time_t fw_ct);
char* fw_time_to_string(fw_time_t time);

//*************************************************************************************************

#endif // FW_TIME_H