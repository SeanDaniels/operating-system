#include "../include/xinu.h"

syscall sl_initlock(sl_lock_t *l){
    static uint32 available_locks = NPSINLOCKS;
    if(!available_locks){
        return SYSERR;
    }
    l->lock_value = UNLOCKED;
    l->lock_owner = NOOWNER;
    available_locks--;
    return OK;
}
syscall sl_lock(sl_lock_t *l){
    while(test_and_set(&(l->lock_value), 1)==1);
    l->lock_owner = currpid;
    return OK;
}
syscall sl_unlock(sl_lock_t *l){
    if(l->lock_owner!=currpid){
       return SYSERR; 
    }
    l->lock_value = UNLOCKED;
    l->lock_owner = NOOWNER;
    return OK;
}
