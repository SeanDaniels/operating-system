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

    l->flag = 1; /* Set flag to 1 (lock is held) */
    l->guard =
        0; /* Set guard to 0, it is now safe for other threads to attempt*/
    l->owner = currpid;
  }
  // some process already holds lock
  else {
    enqueue(currpid, l->queue);
    setPark();
    l->guard = 0;
    park();
  }
  return OK;
}

/* At unlock, if no threads waiting on lock, set lock owner to -1
 * If there are threads waiting on the lock, let the the lock function update
 * lock owner */
syscall al_unlock(al_lock_t *l) {
  while (test_and_set(&(l->guard), 1) == 1)
    ;
  if (isempty(l->queue)) {
    l->flag = 0;
    l->owner = -1;
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
    /* Indicate in the proccess table that this process is waiting on this lock
     */
    prptr->prlock = l->lock_number;
    resched();
    prptr->park_flag = 0;
    restore(mask);
    return OK;
  }
#ifdef LOCK_DBG
  kprintf("Set park has been disabled by some other thread\n");
#endif
  return OK;
}

syscall al_unpark(pid32 thread) {
  int mask;
  mask = disable();
  struct procent *prptr = &proctab[thread];
  prptr->park_flag = 0;
  prptr->prlock = -1;
  ready(thread);
  restore(mask);
  return OK;
}

syscall check_deadlock() { return OK; }
