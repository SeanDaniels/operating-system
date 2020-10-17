#include "../include/xinu.h"
#define DBG
/* LOCKED = 1  */
/* UNLOCKED = 0 */
syscall sl_initlock(sl_lock_t *l){
    static uint32 available_locks = NPSINLOCKS;
    kprintf("Initializing lock\n");
    if(!available_locks){
        return SYSERR;
    }
    l->lock_value = UNLOCKED;
    l->lock_owner = NOOWNER;
    available_locks--;
    return OK;
}
syscall sl_lock(sl_lock_t *l){
    #ifdef DBG
    kprintf("\nThread %d attempting critical section\n", currpid);
    #endif
    while(test_and_set(&(l->lock_value), 1)==1);
    #ifdef DBG
    kprintf("Lock retrieved, setting owner to thread %d\n", currpid);
    #endif
    l->lock_owner = currpid;
    return OK;
}
syscall sl_unlock(sl_lock_t *l){
#ifdef DBG
    kprintf("Thread %d attempting to release lock\n", currpid);
#endif // DBG
    if(l->lock_owner!=currpid){
#ifdef DBG
        kprintf("**Error (thread %d not lock owner)**\n", currpid);
#endif // MACRO
       return SYSERR;
    }
    l->lock_value = UNLOCKED;
    l->lock_owner = (pid32) 0;
#ifdef DBG
    kprintf("Thread %d released lock\n");
#endif
    return OK;
}
