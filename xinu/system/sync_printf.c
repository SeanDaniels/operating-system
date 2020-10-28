#include "../include/xinu.h"


void sync_printf(char *fmt, ...) {
  intmask mask = disable();
  void *arg = __builtin_apply_args();
  __builtin_apply((void *)kprintf, arg, 100);
  restore(mask);
}
