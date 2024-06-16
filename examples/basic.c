#include "fbrs.h"
#include <stdio.h>

int main() {
  volatile int x = 0;

  struct fbrs_context_t c;
  fbrs_save_context(&c);

  printf("hello, world!\n");

  if (x == 0) {
    x++;
    fbrs_load_context(&c);
  }

  return 0;
}
