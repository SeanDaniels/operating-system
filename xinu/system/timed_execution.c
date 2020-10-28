#include "../include/xinu.h"

void run_for_ms(uint32 time) {
  uint32 start = proctab[currpid].runtime;
  while (proctab[currpid].runtime - start < time)
    ;
  kprintf("Process %d finished timed execution\n", currpid);
  return;
}
