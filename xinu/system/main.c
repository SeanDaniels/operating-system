#include "../include/xinu.h"
#include <stdio.h>
#define DBG
void timed_execution(uint32 runtime){
	    while(proctab[currpid].runtime<runtime);
}

void sync_printf(char *fmt, ...)
{
        intmask mask = disable();
        void *arg = __builtin_apply_args();
        __builtin_apply((void*)kprintf, arg, 100);
        restore(mask);
}

int main() {
	pid32 prA, prB;
	sync_printf("================================================================\n\n");
	sync_printf("=== TESTCASE 1::  2 processes - context switches ===============\n");	
	prA = create_user_process(timed_execution, 1024, "timed_execution", 1, 100);
	prB = create_user_process(timed_execution, 1024, "timed_execution", 1, 100);
	resume(prA);
#ifdef DBG
	kprintf("Process A inserted\n");
#endif // DBG
	resume(prB);
#ifdef DBG
	kprintf("Process B inserted\n");
	/* kprintf("Printing HPQ\n"); */
	/* print_queue(q_high); */
	/* kprintf("Printing ready list\n"); */
	/* print_ready_list(); */
#endif // DBG
	receive();
#ifdef DBG
	sync_printf("Receive 1 complete\n");
//	kprintf("\nProcess table after reception:\n");
#endif // DBG
	receive();
#ifdef DBG
	sync_printf("\nReceive 2 complete\n");
//	kprintf("\nProcess table after reception:\n");
//	print_proc();
#endif // DBG
	sleepms(50); //wait for user processes to terminate
#ifdef DBG
	sync_printf("\nprocess %d:: runtime=%d, turnaround time=%d, ctx=%d\n",prA, proctab[prA].runtime, proctab[prA].turnaroundtime, proctab[prA].num_ctxsw);
	sync_printf("process %d:: runtime=%d, turnaround time=%d, ctx=%d\n",prB, proctab[prB].runtime, proctab[prB].turnaroundtime, proctab[prB].num_ctxsw);
	sync_printf("Process table at completion:\n");
//	print_proc();
//	kprintf("\nReady list at completion:\n");
//	print_ready_list();
	sync_printf("\nDone\n");
#endif // DBG
	return OK;
}
