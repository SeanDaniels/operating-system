#include "../include/xinu.h"
#define LOCK_DBG
qid16 lockQueue;
int32 parkFlag = 0;
int32 lockPriority = 200;

syscall initlock(lock_t *l){
    int mask;
    mask = disable();
    l->flag = 0;
    l->guard = 0;
    lockQueue = newqueue();
    l->q =  lockQueue;
    #ifdef LOCK_DBG
    kprintf("Lock's q id: %d\n", lockQueue);
    #endif
    restore(mask);
    return OK;
}
syscall lock(lock_t *l){
    while(test_and_set(&(l->guard), 1) == 1);
    if(l->flag==0){
        l->flag = 1;
        l->guard = 0;
    }
    else{
        setPark();
    #ifdef LOCK_DBG
        kprintf("Lock already held, inserting process %d into lock queue\n", currpid);
    #endif
        l->guard = 0;
        park();
    }
    return OK;
}
syscall unlock(lock_t *l){
    while(test_and_set(&(l->guard), 1) == 1);
    if(isempty(lockQueue)){
    #ifdef LOCK_DBG
        kprintf("Unlock called by process %d\nNo more processes in lock q\n", currpid);
    #endif
        l->flag = 0;
    }
    else{
        unpark(dequeue(lockQueue));
    }
    l->guard = 0;
    return OK;
}
syscall setPark(){
    parkFlag = 1;
    return OK;
}
//indicate about to park
//if another thread calls unpark before called
//subsequent park returns immediately instead of sleeping
syscall park(){
    int mask = disable();
    struct procent *prptr;
    if(parkFlag==0){
#ifdef LOCK_DBG
        kprintf("Set park has been disabled by some other thread\n");
#endif
        return OK;
    }

    prptr = &proctab[currpid];
    prptr->prstate = PR_WAIT;
    enqueue(currpid, lockQueue);
    resched();
    resume(mask);
    return OK;
}

syscall unpark(pid32 thread){
    int mask = disable();
    struct procent *prptr;
    parkFlag = 0;
    #ifdef LOCK_DBG
        kprintf("Unlocking, unsleeping process %d\n", thread);
    #endif
    prptr = &proctab[thread];
    prptr->prstate = PR_READY;
    ready(thread);
    resume(mask);
    return OK;
}
