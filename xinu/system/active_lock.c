#include "../include/xinu.h"

#define LOCK_DBG

al_lock_t *al_locktable[NALOCKS];

syscall al_initlock(al_lock_t *l) {
  static uint32 al_locknumber = 0;
  if (al_locknumber < NALOCKS) {
    int mask;
    mask = disable();
    l->flag = 0;
    l->guard = 0;
    l->owner = -1;
    l->lock_number = al_locknumber++;
    l->queue = newqueue();
    al_locktable[al_locknumber] = l;
    restore(mask);
    return OK;
  }
#ifdef LOCK_DBG
  kprintf("No more space for locks\n");
#endif
  return SYSERR;
}

syscall al_lock(al_lock_t *l) {
  while (test_and_set(&(l->guard), 1) == 1)
    ;
  // no process holds lock
  if (l->flag == 0) {
    l->flag = 1;
    l->guard = 0;
    l->owner = currpid;
  }
  // some process already holds lock
  else {
    enqueue(currpid, l->queue);
    setPark();
    l->guard = 0;
    al_park(l);
  }
  return OK;
}

syscall al_unlock(al_lock_t *l) {
  while (test_and_set(&(l->guard), 1) == 1)
    ;
  if (isempty(l->queue)) {
    l->flag = 0;
  } else {
    unpark(dequeue(l->queue));
  }
  l->guard = 0;
  return OK;
}

syscall al_park(al_lock_t *l) {
  int mask;
  mask = disable();
  struct procent *prptr;
  prptr = &proctab[currpid];
  if (prptr->park_flag == 1) {
    prptr->prstate = PR_WAIT;
    prptr->prlock = l->lock_number;
#ifdef LOCK_DBG
    kprintf("Process %d has been parked, waiting on lock %X\n", currpid,
            prptr->prlock);
    kprintf("Lock %X is owned by process %d\n", l->lock_number, l->owner);
#endif
    resched();
    prptr->park_flag = 0;
    restore(mask);
    return OK;
  }
  return OK;
}

syscall check_deadlock() { return OK; }
