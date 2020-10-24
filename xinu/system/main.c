#include "../include/xinu.h"
#include <stdio.h>
#define DBG
#define DEADLOCK
void sync_printf(char *fmt, ...) {
  intmask mask = disable();
  void *arg = __builtin_apply_args();
  __builtin_apply((void *)kprintf, arg, 100);
  restore(mask);
}

void run_for_ms(uint32 time) {
  uint32 start = proctab[currpid].runtime;
  while (proctab[currpid].runtime - start < time)
    ;
}
/* pass shared variable, lock */

process lock_routine_a(al_lock_t *l_a, al_lock_t *l_b) {
  uint32 i;
  for (i = 0; i < 2; i++) {
    al_lock(l_a);
#ifdef DBG
    kprintf("Process %d has aquired lock %X\n", currpid, l_a->lock_number);
#endif
    run_for_ms(1000);
    al_lock(l_b);
#ifdef DBG
    kprintf("Process %d has aquired lock %X\n", currpid, l_b->lock_number);
#endif
    run_for_ms(1000);
    al_unlock(l_b);
    al_unlock(l_a);
  }
  return OK;
}

process lock_routine_b(al_lock_t *l_a, al_lock_t *l_b) {
  uint32 i;
  for (i = 0; i < 2; i++) {
    al_lock(l_b);
#ifdef DBG
    kprintf("Process %d has aquired lock %X\n", currpid, l_b->lock_number);
#endif
    run_for_ms(1000);
    al_lock(l_a);
#ifdef DBG
    kprintf("Process %d has aquired lock %X\n", currpid, l_a->lock_number);
#endif
    run_for_ms(1000);
    al_unlock(l_a);
    al_unlock(l_b);
  }
  return OK;
}

process main(void) {
  pid32 pid1, pid2;
  al_lock_t l_a, l_b;
  al_initlock(&l_a);
  al_initlock(&l_b);
  pid1 = create((void *)lock_routine_a, INITSTK, 1, "lr_a", 2, &l_a, &l_b);
  pid2 = create((void *)lock_routine_b, INITSTK, 1, "lr_b", 2, &l_a, &l_b);
  resume(pid1);
  sleepms(500);
  resume(pid2);
  receive();
  receive();
  sync_printf("\nDone\n");
  return OK;
}
