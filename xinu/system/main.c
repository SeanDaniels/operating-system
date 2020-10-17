#include "../include/xinu.h"
#include <stdio.h>
#define DBG

void sync_printf(char *fmt, ...)
{
        intmask mask = disable();
        void *arg = __builtin_apply_args();
        __builtin_apply((void*)kprintf, arg, 100);
        restore(mask);
}

/* pass shared variable, lock */
process increment(uint32 *x, uint32 n, sl_lock_t *mutex){
	uint32 i, j;
	for (i=0; i<n; i++){
        kprintf("\nThread %d before critical section\n", currpid);
		sl_lock(mutex);
        kprintf("\nThread %d in critical section-->\n", currpid);
		(*x)+=1;
		for (j=0; j<1000; j++);
		yield();
        kprintf("-->Thread %d exiting critical section\n", currpid);
		sl_unlock(mutex);
        kprintf("Thread %d has unlocked\n", currpid);
	}
    return OK;
}


process nthreads(uint32 nt, uint32 *x, uint32 n, sl_lock_t *mutex){
	pid32 pids[nt];
	int i;
	for (i=0; i < nt; i++){
		pids[i] = create((void *)increment, INITSTK, 1,"p", 3, x, n, mutex);
		if (pids[i]==SYSERR){
			kprintf("nthreads():: ERROR - could not create process");
			return SYSERR;
		}
	}
	for (i=0; i < nt; i++){
		if (resume(pids[i]) == SYSERR){
			kprintf("nthreads():: ERROR - could not resume process");
			return SYSERR;
		}
	}
    for (i=0; i < nt; i++){
        kprintf("Receiving %d\n", i);
        receive();
    }
	return OK;
}


process	main(void)
{
    uint32 old_val, new_val, ret_val;
    uint32 *pointer_to_old_val;
    pid32 owner;

    uint32 x;
    unsigned nt, value=10;
    sl_lock_t mutex;

	kprintf("\n\n=====     Testing the SPINLOCK's implementation     =====\n");
    x = 0;
    nt = 10;
    // unlocked = 0
    // locked = 1
    sl_initlock(&mutex);
	resume(create((void *)nthreads, INITSTK, 1,"nthreads", 4, nt, &x, value/nt, &mutex));
    receive();
    sync_printf("%d threads, n = %d, target value = %d\n", nt, value, x);
	if (x==value) kprintf("TEST PASSED.\n"); else kprintf("TEST FAILED.\n");

/* basic mutex testing  */
/*     old_val = mutex.lock_value; */
/*     owner = mutex.lock_owner; */
/*     sync_printf("\nSpinlock created, lock value = %d, lock owner = %X\n", old_val, owner); */
/*     sl_lock(&mutex); */
/*     owner = mutex.lock_owner; */
/*     old_val = mutex.lock_value; */
/*     sync_printf("\nSpinlock locked, lock value = %d, lock owner = %X\n", old_val, owner); */
/* end of basic mutex testing */

    /* returnVal = test_and_set(pointer_to_old_val, new_val); */
    /* sync_printf("Return value: %x\n", returnVal); */
    sync_printf("\nMain Process Done\n");

    return OK;
    
}
