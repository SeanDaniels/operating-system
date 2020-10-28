#include "../include/xinu.h"
#define DBG

process priority_lock_routine_a(pi_lock_t *l_a) {
  pi_lock(l_a);
#ifdef DBG
  kprintf("Process %d has aquired lock %X\n", currpid, l_a->lock_number);
#endif
  run_for_ms(1000);
  pi_unlock(l_a);
#ifdef DBG
  kprintf("Process %d has released lock %X\n", currpid, l_a->lock_number);
#endif
#ifdef DBG
  kprintf("Lock routine 'a' finished\n");
#endif
  return OK;
}

process priority_lock_routine_b(pi_lock_t *l_a) {
  pi_lock(l_a);
#ifdef DBG
  kprintf("Process %d has aquired lock %X\n", currpid, l_a->lock_number);
#endif
  run_for_ms(1000);
  pi_unlock(l_a);
#ifdef DBG
  kprintf("Process %d has released lock %X\n", currpid, l_a->lock_number);
#endif
#ifdef DBG
  kprintf("Lock routine 'b' finished\n");
#endif
  return OK;
}
process medium_priority_thread(uint32 number_of_processes, uint32 run_time) {
  pid32 medium_priority_threads[10];
  int i;
  for (i = 0; i < number_of_processes; i++) {
    medium_priority_threads[i] =
        create((void *)run_for_ms, INITSTK, 3, "p", 1, run_time);
    if (medium_priority_threads[i] == SYSERR) {
      kprintf("Error creating medium priority process %d\n", i);
      return SYSERR;
    }
  }
  for (i = 0; i < number_of_processes; i++) {
    if (resume(medium_priority_threads[i]) == SYSERR) {
      kprintf("Error resuming medium priority process %d\n", i);
      return SYSERR;
    }
  }
  for (i = 0; i < number_of_processes; i++)
    receive();
  return OK;
}

process main(void) {
  // pid32 pid1, pid2;
  pi_lock_t l_a;
  uint32 number_of_medium_priority_processes = 10,
         medium_pri_process_runtime = 1000;

  // Lower priority process has lock
  // Higher priority process needs lock
  pi_initlock(&l_a);
  resume(create((void *)priority_lock_routine_a, INITSTK, 1, "lr_a", 1, &l_a));
  sleepms(10);
  resume(create(
      (void *)medium_priority_thread, INITSTK, 4, "medium_priority_process", 2,
      number_of_medium_priority_processes, medium_pri_process_runtime));
  resume(create((void *)priority_lock_routine_b, INITSTK, 5, "lr_b", 1, &l_a));
  receive();
  receive();
  receive();
  sync_printf("\nDone\n");
  return OK;
}
