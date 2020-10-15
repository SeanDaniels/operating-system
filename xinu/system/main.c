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
    uint32 old_val = 4;
    uint32 *pointer_to_old_val = &old_val;
    uint32 new_val = 1;
    uint32 returnVal;

    sync_printf("\nMain Process Start\n");

    returnVal = test_and_set(pointer_to_old_val, new_val);

    sync_printf("Return value: %x\n", returnVal);
    sync_printf("\nMain Process Done\n");

    return OK;
    
}
