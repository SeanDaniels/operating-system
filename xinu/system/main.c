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


process	main(void)
{
    uint32 old_val, new_val, ret_val;
    uint32 *pointer_to_old_val;
    pid32 owner;
    sl_lock_t mutex;

    sync_printf("\nMain Process Start\n");
    sync_printf("\nCreating spinlock\n");
    // unlocked = 0
    // locked = 1
    sl_initlock(&mutex);
    old_val = mutex.lock_value;
    owner = mutex.lock_owner;
    sync_printf("\nSpinlock created, lock value = %d, lock owner = %X\n", old_val, owner);
    sl_lock(&mutex);
    owner = mutex.lock_owner;
    old_val = mutex.lock_value;
    sync_printf("\nSpinlock locked, lock value = %d, lock owner = %X\n", old_val, owner);



    /* returnVal = test_and_set(pointer_to_old_val, new_val); */
    /* sync_printf("Return value: %x\n", returnVal); */
    sync_printf("\nMain Process Done\n");

    return OK;
    
}
