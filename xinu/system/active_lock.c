#include "../include/xinu.h"
#define NOOWNER 200
#define LOCK_DBG
#define PARK_DBG
al_lock_t *al_locktable[NALOCKS];
pid32 rootProcess;

syscall al_initlock(al_lock_t *l) {
  static uint32 al_locknumber = 0;
  if (al_locknumber < NALOCKS) {
    int mask;
    mask = disable();
    l->flag = 0;
    l->guard = 0;
    l->owner = NOOWNER;
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
  int mask;
  while (test_and_set(&(l->guard), 1) == 1)
    ;
  // no process holds lock
  mask = disable();
  check_deadlock(l);
  restore(mask);
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
    l->owner = NOOWNER;
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
#ifdef PARK_DBG
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

syscall has_lock(pid32 process) {
  uint32 i;
  al_lock_t *lockptr;
#ifdef LOCK_DBG
  kprintf("Checking if process %d owns any locks\n", process);
#endif
  for (i = 0; i < NALOCKS; i++) {
    lockptr = al_locktable[i];
    if (lockptr->owner == process) {
#ifdef LOCK_DBG
      kprintf("Process %d owns lock %d\n", process, lockptr->lock_number);
#endif
      has_client(lockptr->lock_number);
    }
  }
  return OK;
}

syscall has_client(uint32 lockNumber) {
  uint32 i;
  struct procent *prptr;
#ifdef LOCK_DBG
  kprintf("Checking if any processes are waiting on lock %d\n", lockNumber);
#endif
  for (i = 0; i < NPROC; i++) {
    prptr = &proctab[i];
    if (prptr->prlock == lockNumber) {
#ifdef LOCK_DBG
      kprintf("Process %d is waiting on lock %d\n", i, lockNumber);
#endif
      if (i == rootProcess) {
#ifdef LOCK_DBG
        kprintf("Process %d is root process, circ. deb found\n", i);
#endif
        return OK;
      }
      has_lock(i);
    }
  }
  return OK;
}

syscall check_deadlock(al_lock_t *l) {
  rootProcess = currpid;
#ifdef LOCK_DBG
  kprintf("**Beginning Deadlock Check for lock %d**\n", l->lock_number);
#endif
  has_lock(currpid);
#ifdef LOCK_DBG
  kprintf("**Deadlock Check Complete**\n");
#endif
  return OK;
}
