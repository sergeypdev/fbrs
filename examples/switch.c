#include <stdint.h>
#include <stdio.h>

#include "fbrs.h"

static struct fbrs_context_t main_context;

void fiber_entry() {
  for (int i = 0; i < 10; i++) {
    printf("Hello, fibers %d!\n", i);
    fbrs_switch_context(fbrs_current_context(), &main_context);
  }
}

#define STACK_SIZE (4096)

int main(int argc, char **argv) {
  char stack[STACK_SIZE];

  void *sp = fbrs_get_stack_pointer((void *)stack, STACK_SIZE);

  struct fbrs_context_t context = {0};

  context.ip = (void *)fiber_entry;
  context.sp = (void *)sp;

  for (int i = 0; i < 5; i++) {
    fbrs_switch_context(&main_context, &context);
    printf("Hello, main %d!\n", i);
  }

  printf("Ok, that's enough\n");

  return 0;
}
