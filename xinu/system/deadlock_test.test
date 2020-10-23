#include "../include/xinu.h"
#include <stdio.h>
#define DBG
//#define LOCKOUTPUT
//#define RECEIVEOUTPUT
//#define SP_TESTCASE1
//#define SP_TESTCASE2
//#define SP_TESTCASE3
//#define SP_TESTCASE4
//#define SP_TESTCASE5
//#define L_TESTCASE1
//#define L_TESTCASE2
#define L_TESTCASE3
void sync_printf(char *fmt, ...) {
  intmask mask = disable();
  void *arg = __builtin_apply_args();
  __builtin_apply((void *)kprintf, arg, 100);
  restore(mask);
}

/* pass shared variable, lock */
process increment(uint32 *x, uint32 n, al_lock_t *mutex) {
  uint32 i, j;
  for (i = 0; i < n; i++) {
    lock(mutex);
#ifdef LOCKOUTPUT
    kprintf("\nThread %d in critical section-->\n", currpid);
#endif
    (*x) += 1;
    for (j = 0; j < 1000; j++)
      ;
    yield();
#ifdef LOCKOUTPUT
    kprintf("-->Thread %d exiting critical section\n", currpid);
#endif // LOCKOUTPUT
    unlock(mutex);
#ifdef LOCKOUTPUT
    kprintf("Thread %d has unlocked\n", currpid);
#endif
  }
  return OK;
}

process nthreads(uint32 nt, uint32 *x, uint32 n, al_lock_t *mutex) {
  pid32 pids[nt];
  int i;
  for (i = 0; i < nt; i++) {
    pids[i] = create((void *)increment, INITSTK, 1, "p", 3, x, n, mutex);
    if (pids[i] == SYSERR) {
      kprintf("nthreads():: ERROR - could not create process");
      return SYSERR;
    }
  }
  for (i = 0; i < nt; i++) {
    if (resume(pids[i]) == SYSERR) {
      kprintf("nthreads():: ERROR - could not resume process");
      return SYSERR;
    }
  }
  for (i = 0; i < nt; i++) {
#ifdef RECEIVEOUTPUT
    kprintf("Attempting to receive %d\n", i);
#endif
    receive();
#ifdef RECEIVEOUTPUT
    kprintf("Received %d\n", i);
#endif
  }
  return OK;
}

process main(void) {
  pid32 owner;

  uint32 x;
  unsigned nt;
  unsigned value = 1000000;
  lock_t mutex;

  //    kprintf("\n\n=====     Testing the SPINLOCK's implementation =====\n");
  kprintf("\n\n=====       Testing the LOCK w/ sleep&guard         =====\n");
#ifdef L_TESTCASE1
  kprintf("\n\n================= TEST 1 = 10 threads ===================\n");
  x = 0;
  nt = 10;
  initlock(&mutex);
  resume(create((void *)nthreads, INITSTK, 1, "nthreads", 4, nt, &x, value / nt,
                &mutex));
  receive();
  sync_printf("%d threads, n=%d, target value=%d\n", nt, value, x);
  if (x == value)
    kprintf("TEST PASSED.\n");
  else
    kprintf("TEST FAILED.\n");
#endif

#ifdef L_TESTCASE2
  // 20 threads
  kprintf("\n\n================= TEST 2 = 20 threads ===================\n");
  x = 0;
  nt = 20;
  initlock(&mutex);
  resume(create((void *)nthreads, INITSTK, 1, "nthreads", 4, nt, &x, value / nt,
                &mutex));
  receive();
  sync_printf("%d threads, n=%d, target value=%d\n", nt, value, x);
  if (x == value)
    kprintf("TEST PASSED.\n");
  else
    kprintf("TEST FAILED.\n");
#endif
#ifdef L_TESTCASE3
  kprintf("\n\n================= TEST 3 = 50 threads ===================\n");
  x = 0;
  nt = 50;
  kprintf("Address of mutex: %X\n", &mutex);
  initlock(&mutex);
  resume(create((void *)nthreads, INITSTK, 1, "nthreads", 4, nt, &x, value / nt,
                &mutex));
  receive();
  sync_printf("%d threads, n=%d, target value=%d\n", nt, value, x);
  if (x == value)
    kprintf("TEST PASSED.\n");
  else
    kprintf("TEST FAILED.\n");
#endif
#ifdef SP_TESTCASE1
  kprintf("\n\n=====     Test 1 ---- 10 threads =====\n");
  x = 0;
  nt = 10;
  sl_initlock(&mutex);
  resume(create((void *)nthreads, INITSTK, 1, "nthreads", 4, nt, &x, value / nt,
                &mutex));
  receive();
  sync_printf("%d threads, n = %d, target value = %d\n", nt, value, x);
  if (x == value)
    kprintf("TEST PASSED.\n");
  else
    kprintf("TEST FAILED.\n");
#endif

#ifdef SP_TESTCASE2
  kprintf("\n\n=====     Test 2 ---- 20 threads =====\n");
  x = 0;
  nt = 20;
  sl_initlock(&mutex);
  resume(create((void *)nthreads, INITSTK, 1, "nthreads", 4, nt, &x, value / nt,
                &mutex));
  receive();
  sync_printf("%d threads, n=%d, target value=%d\n", nt, value, x);
  if (x == value)
    kprintf("TEST PASSED.\n");
  else
    kprintf("TEST FAILED.\n");
#endif

#ifdef SP_TESTCASE3
  kprintf("\n\n=====     Test 3 ---- 25 threads =====\n");
  x = 0;
  nt = 25;
  sl_initlock(&mutex);
  resume(create((void *)nthreads, INITSTK, 1, "nthreads", 4, nt, &x, value / nt,
                &mutex));
  receive();
  sync_printf("%d threads, n=%d, target value=%d\n", nt, value, x);
  if (x == value)
    kprintf("TEST PASSED.\n");
  else
    kprintf("TEST FAILED.\n");
#endif

#ifdef SP_TESTCASE4
  kprintf("\n\n================= TEST 4 = 40 threads ===================\n");
  x = 0;
  nt = 40;
  sl_initlock(&mutex);
  resume(create((void *)nthreads, INITSTK, 1, "nthreads", 4, nt, &x, value / nt,
                &mutex));
  receive();
  sync_printf("%d threads, n=%d, target value=%d\n", nt, value, x);
  if (x == value)
    kprintf("TEST PASSED.\n");
  else
    kprintf("TEST FAILED.\n");
#endif

#ifdef SP_TESTCASE5
  kprintf("\n\n================= TEST 5 = 50 threads ===================\n");
  x = 0;
  nt = 50;
  sl_initlock(&mutex);
  resume(create((void *)nthreads, INITSTK, 1, "nthreads", 4, nt, &x, value / nt,
                &mutex));
  receive();
  sync_printf("%d threads, n=%d, target value=%d\n", nt, value, x);
  if (x == value)
    kprintf("TEST PASSED.\n");
  else
    kprintf("TEST FAILED.\n");
#endif

  sync_printf("\nMain Process Done\n");
  return OK;
}
