#include "../include/xinu.h"

//#define LOCK_DBG
#define PARK_DBG

al_lock_t *al_locktable[NALOCKS];

pid32 rootProcess;

uint32 rootLock;
// Change process prlock entry to lock's number, not lock pointer

syscall al_initlock(al_lock_t *l) {
  static uint32 al_locknumber = 0;
  if (al_locknumber < NALOCKS) {
    int mask;
    mask = disable();
    l->flag = 0;
    l->guard = 0;
    l->owner = NOOWNER;
    l->lock_number = al_locknumber;
    l->queue = newqueue();
    al_locktable[al_locknumber] = l;
    al_locknumber++;
    restore(mask);
    return OK;
  }
#ifdef LOCK_DBG
  kprintf("No more space for locks\n");
#endif
  return SYSERR;
}

syscall al_lock(al_lock_t *l) {
  pid32 dep_check_result;
  int mask, dep = 0;
  while (test_and_set(&(l->guard), 1) == 1)
    ;
  mask = disable();
#ifdef LOCK_DBG
  kprintf("Circular dep check:\nprocess %d | lock %d\n", currpid,
          l->lock_number);
#endif
  // returns the root process if there is a deadlock
  dep_check_result = get_owner(l, currpid);
  if (dep_check_result == currpid) {
#ifdef LOCK_DBG
    kprintf("Circular dep located\n");
#endif
  }
  restore(mask);
  // no process holds lock
  if (l->flag == 0) {
    l->flag = 1;
    l->owner = currpid;
    l->guard = 0;
  }
  // some process already holds lock
  else {
    enqueue(currpid, l->queue);
    mask = disable();
    if (dep_check_result == currpid) {
      dep = 1;
    }
    restore(mask);
    setPark();
    l->guard = 0;
    al_park(l, dep);
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
  deadlock_chain[l->lock_number] = FALSE;
  l->owner = NOOWNER;
  l->guard = 0;
  return OK;
}

syscall al_park(al_lock_t *l, int dep) {
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
    if (dep)
      print_deadlock();
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
  prptr->prlock = NOTWAITING;
  ready(thread);
  restore(mask);
  return OK;
}

int32 get_owner(al_lock_t *l, pid32 rootProcess) {
  pid32 owner = l->owner;
  int32 waiting_lock_number;
  al_lock_t *next_lock;
  int32 result;
  if (owner == NOOWNER) {
    return -1;
  }
  if (owner == rootProcess) {
    // kprintf("Lock %d is owned by process %d\n", l->lock_number, rootProcess);
    deadlock_chain[owner] = TRUE;
#ifdef LOCK_DBG
    kprintf("%d --|owns|--> %d\n", owner, l->lock_number);
#endif
    return rootProcess;
  }
  waiting_lock_number = proctab[owner].prlock;
  if (waiting_lock_number == NOTWAITING) {
    return -1;
  }
  next_lock = al_locktable[waiting_lock_number];
  result = get_owner(next_lock, rootProcess);
  if (result == rootProcess) {
    deadlock_chain[owner] = TRUE;
#ifdef LOCK_DBG
    kprintf("%d --|owns|--> %d --|waits|--> %d\n", owner, l->lock_number,
            waiting_lock_number);
#endif
    return rootProcess;
  }
  return -1;
}

bool8 al_trylock(al_lock_t *l) {
  if (l->owner == NOOWNER) {
    return TRUE;
  }
  return FALSE;
}

void print_deadlock() {
  uint32 i;
  uint32 first = 1;
  kprintf("lock_detected<");
  for (i = 0; i < NPROC; i++) {
    if (deadlock_chain[i] == TRUE) {
      deadlock_chain[i] = FALSE;
      if (first) {
        kprintf("P%d", i);
        first = 0;
      } else
        kprintf("-P%d", i);
    }
  }
  kprintf(">\n");
  return;
}
