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

  char *sp = (char *)(stack + STACK_SIZE);
  sp = (char *)((uintptr_t)sp & -16L);
  sp -= 128;

  struct fbrs_context_t context = {0};

  context.rip = (void *)fiber_entry;
  context.rsp = (void *)sp - 8;

  for (int i = 0; i < 5; i++) {
    fbrs_switch_context(&main_context, &context);
    printf("Hello, main %d!\n", i);
  }

  printf("Ok, that's enough\n");

  return 0;
}
