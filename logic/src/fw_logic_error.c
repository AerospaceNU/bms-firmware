#include <fcntl.h>

void log_init() {
  close(1);
  int fd = open("/logs/fw_pico_log.log", O_APPEND, 0777);
}

void log_error(const char *file_name, int line_num, const char *msg) {
    // TODO: Implement this function
}  