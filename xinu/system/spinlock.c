#include "../include/xinu.h"
//#define DBG
//#define LOCKOUTPUT
/* LOCKED = 1  */
/* UNLOCKED = 0 */
syscall sl_initlock(sl_lock_t *l){
    static uint32 available_locks = NPSINLOCKS;
#ifdef LOCKOUTPUT
    kprintf("Initializing lock\n");
#endif
    if(!available_locks){
        return SYSERR;
    }
    l->lock_value = UNLOCKED;
    l->lock_owner = NOOWNER;
    available_locks--;
    return OK;
}
syscall sl_lock(sl_lock_t *l){
#ifdef LOCKOUTPUT
    kprintf("\nThread %d attempting critical section\n", currpid);
#endif
    while(test_and_set(&(l->lock_value), 1)==1);
#ifdef LOCKOUTPUT
    kprintf("Lock retrieved, setting owner to thread %d\n", currpid);
#endif
    l->lock_owner = currpid;
    return OK;
}
syscall sl_unlock(sl_lock_t *l){
#ifdef LOCKOUTPUT
    kprintf("Thread %d attempting to release lock\n", currpid);
#endif
    if(l->lock_owner!=currpid){
#ifdef LOCKOUTPUT
        kprintf("**Error (thread %d not lock owner)**\n", currpid);
#endif
       return SYSERR;
    }
    l->lock_value = UNLOCKED;
    l->lock_owner = (pid32) 0;
#ifdef LOCKOUTPUT
    kprintf("Thread %d released lock\n");
#endif
    return OK;
}
