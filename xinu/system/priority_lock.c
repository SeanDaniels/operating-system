#include "../include/xinu.h"
pi_lock_t *pi_locktable[NPILOCKS];

syscall pi_initlock(pi_lock_t *l) {
  static uint32 pi_locknumber = 0;
  if (pi_locknumber < NPILOCKS) {
    int mask;
    mask = disable();
    l->flag = 0;
    l->guard = 0;
    l->owner = NOOWNER;
    l->lock_number = pi_locknumber;
    l->queue = newqueue();
    pi_locktable[pi_locknumber] = l;
    pi_locknumber++;
    restore(mask);
    return OK;
  }
#ifdef LOCK_DBG
  kprintf("No more space for locks\n");
#endif
  return SYSERR;
}

syscall pi_lock(pi_lock_t *l) {
  pid32 dep_check_result;
  int mask;
  bool8 prio = FALSE;
  while (test_and_set(&(l->guard), 1) == 1)
    ;
  // no process holds lock
  if (l->flag == 0) {
    l->flag = 1;
    l->owner = currpid;
    l->guard = 0;
    l->og_priority = proctab[currpid].prprio;
  }
  // some process already holds lock
  else {
    enqueue(currpid, l->queue);
    mask = disable();
    prio = check_priority(l, currpid);
    restore(mask);
    setPark();
    l->guard = 0;
    pi_park(l, prio);
  }
  return OK;
}

syscall pi_unlock(pi_lock_t *l) {
  while (test_and_set(&(l->guard), 1) == 1)
    ;
  if (isempty(l->queue)) {
    l->flag = 0;
  } else {
    unpark(dequeue(l->queue));
    revert_priority(l);
  }
  l->owner = NOOWNER;
  l->guard = 0;
  return OK;
}

syscall pi_park(pi_lock_t *l, bool8 prio) {
  int mask;
  mask = disable();
  struct procent *prptr;
  prptr = &proctab[currpid];
  if (prptr->park_flag == 1) {
    prptr->prstate = PR_WAIT;
    prptr->prlock = l->lock_number;
#ifdef PARK_DBG
    kprintf("Process %d has been parked, waiting on lock %X\n", currpid,
            prptr->prlock);
    kprintf("Lock %X is owned by process %d\n", l->lock_number, l->owner);
#endif
    if (prio) {
#ifdef LOCK_DBG
      kprintf("Print priority shift here\n");
#endif
      // print_priority_shift();
    }
    resched();
    prptr->park_flag = 0;
    restore(mask);
    return OK;
  }
  return OK;
}

syscall pi_unpark(pid32 thread) {
  int mask;
  mask = disable();
  struct procent *prptr = &proctab[thread];
  prptr->park_flag = 0;
  prptr->prlock = NOTWAITING;
  ready(thread);
  restore(mask);
  return OK;
}

/* void print_priority_shift() { */
/*   uint32 i; */
/*   uint32 first = 1; */
/*   kprintf("lock_detected<"); */
/*   for (i = 0; i < NPROC; i++) { */
/*     if (deadlock_chain[i] == TRUE) { */
/*       deadlock_chain[i] = FALSE; */
/*       if (first) { */
/*         kprintf("P%d", i); */
/*         first = 0; */
/*       } else */
/*         kprintf("-P%d", i); */
/*     } */
/*   } */
/*   kprintf(">\n"); */
/*   return; */
/* } */

bool8 check_priority(pi_lock_t *l, pid32 currpid) {
  struct procent *waiting_prptr = &proctab[currpid];
  struct procent *current_prptr = &proctab[l->owner];
  if (waiting_prptr->prprio > current_prptr->prprio) {
    l->inherited_priority = TRUE;
    current_prptr->prprio = waiting_prptr->prprio;
    return TRUE;
  }
  return FALSE;
}

void revert_priority(pi_lock_t *l) {
  int mask;
  mask = disable();
  struct procent *owner;
  if (l->inherited_priority) {
    owner = &proctab[l->owner];
    owner->prprio = l->og_priority;
  }
  restore(mask);
  return;
}
