#include "../include/xinu.h"
//#define LOCK_DBG

syscall initlock(lock_t *l) {
  int mask;
  mask = disable();
#ifdef LOCK_DBG
  kprintf("Mutex passed: %X, Address of mutex passed: %X\n", l, &l);
#endif
  l->flag = 0;
  l->guard = 0;
  l->queue = newqueue();
  restore(mask);
  return OK;
}

syscall lock(lock_t *l) {
  while (test_and_set(&(l->guard), 1) == 1)
    ;
  if (l->flag == 0) {
    l->flag = 1;
    l->guard = 0;
  } else {
    enqueue(currpid, l->queue);
    setPark();
    l->guard = 0;
    park();
  }
  return OK;
}

syscall unlock(lock_t *l) {
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

syscall setPark() {
  int mask;
  mask = disable();
  struct procent *prptr = &proctab[currpid];
#ifdef LOCK_DBG
  kprintf("Setting park flag\n");
#endif
  prptr->park_flag = 1;
  restore(mask);
  return OK;
}

syscall park() {
  int mask;
  mask = disable();
  struct procent *prptr;
  prptr = &proctab[currpid];
  if (prptr->park_flag == 1) {
    prptr->prstate = PR_WAIT;
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

syscall unpark(pid32 thread) {
  int mask;
  mask = disable();
  struct procent *prptr = &proctab[thread];
  prptr->park_flag = 0;
  ready(thread);
  restore(mask);
  return OK;
}
