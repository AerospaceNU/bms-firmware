#include "pico/stdlib.h"

int main() {
  if (stdio_init_all() == false) {
    // print_error E_PICO_IO
  }

  return 0;
}