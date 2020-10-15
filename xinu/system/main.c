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
	/* Run the Xinu shell */
	uint32 lockVal = 0;
	uint32 *contentOfLockVal = &lockVal;
    uint32 old_val = 4;
	uint32 new_val = 1;
	uint32 returnVal;

	kprintf("\nMain Process Start\n");

    returnVal = test_and_set(old_val, new_val);

	kprintf("\nMain Process Done\n");

	return OK;
    
}
