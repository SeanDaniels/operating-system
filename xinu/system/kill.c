/* kill.c - kill */

#include "../include/xinu.h"
#define DBG
/*------------------------------------------------------------------------
 *  kill  -  Kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
syscall	kill(
	  pid32		pid		/* ID of process to kill	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	struct node* listNode;
	int32	i;			/* Index into descriptors	*/

	mask = disable();

	if (isbadpid(pid) || (pid == NULLPROC)
	    || ((prptr = &proctab[pid])->prstate) == PR_FREE) {
		restore(mask);
		return SYSERR;
	}

#ifdef DBG
	if(prptr->prprio==USERPROCESSPRIO){
		kprintf("\nUser process in kill function\n");
	}
#endif

	if (--prcount <= 1) {		/* Last user process completes	*/
		xdone();
	}

	send(prptr->prparent, pid); /*  What does this do? */

	for (i=0; i<3; i++) {
		close(prptr->prdesc[i]);
	}

	freestk(prptr->prstkbase, prptr->prstklen); /*  clear stack space */

	/*  Take action based on process state */
	switch (prptr->prstate) {
	/*  If process is the currently running process, set state to free and reschedule */
	case PR_CURR:
		if(prptr->prprio==USERPROCESSPRIO){
			prptr->turnaroundtime = (clktime*1000+ctr1000)-(prptr->turnaroundtime);
#ifdef DBG
			kprintf("\nUser process (PID %d) suicide\n", pid);
			kprintf("Runtime of process: %d\n", prptr->runtime);
#endif
		}
		prptr->prstate = PR_FREE;	/* Suicide */
		resched();
	case PR_SLEEP:
	case PR_RECTIM:
#ifdef DBG
		if(prptr->prprio==USERPROCESSPRIO){
			kprintf("\nUser process killed while asleep\n");
			//print_queue(q_high);
		}
#endif
		unsleep(pid);
		prptr->prstate = PR_FREE;
		break;

	case PR_WAIT:
		if(prptr->prprio==USERPROCESSPRIO){
			kprintf("\nUser process killed while waiting\n");
			//print_queue(q_high);
		}
		semtab[prptr->prsem].scount++;
		/* Fall through */

	/*  If process is in the ready list, remove from ready list */
	case PR_READY:
#ifdef DBG
		if(prptr->prprio==USERPROCESSPRIO){
			kprintf("\nUser process killed while ready\n");
		}
#endif
		getitem(pid);		/* Remove from queue */
		/* Fall through */
	/*  not one of the above cases, set proctab entry state to free*/
	default:
#ifdef DBG
		if(prptr->prprio==USERPROCESSPRIO){
			kprintf("\nUser process killed default\n");
		}
#endif
		prptr->prstate = PR_FREE;
	}
	restore(mask);
	return OK;
}
