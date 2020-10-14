/* ready.c - ready */

#include "../include/xinu.h"
//#define DBG
#define MLFQ

qid16	readylist;			/* Index of ready list		*/
qid16 q_high;
qid16 q_med;
qid16 q_low;
/*------------------------------------------------------------------------
 *  ready  -  Make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
status	ready(
	  pid32		pid		/* ID of process to make ready	*/
	)
{
	register struct procent *prptr;

	if (isbadpid(pid)) {
		return SYSERR;
	}

	/* Set process state to indicate ready and add to ready list */
	prptr = &proctab[pid];
	prptr->prstate = PR_READY;
	if(prptr->prprio==USERPROCESSPRIO){
		if(prptr->turnaroundtime==NULL){
			prptr->turnaroundtime=(clktime*1000)+ctr1000;
		}
#ifdef MLFQ
		kprintf("Inserting user process into HPQ\n");
		insert(pid, q_high, prptr->prprio);
		qInfo[pid].currentQ = 0;
		qInfo[pid].allotment = 0;
		kprintf("Process inserted\n");
		resched();
		return OK;
#endif // MLFQ
	}
	insert(pid, readylist, prptr->prprio);
	resched();
	return OK;
}
