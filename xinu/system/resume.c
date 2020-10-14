/* resume.c - resume */

#include "../include/xinu.h"
//#define DBG
/*------------------------------------------------------------------------
 *  resume  -  Unsuspend a process, making it ready
 *------------------------------------------------------------------------
 */
pri16	resume(
	  pid32		pid		/* ID of process to unsuspend	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	pri16	prio;			/* Priority to return		*/

	mask = disable();
	if (isbadpid(pid)) {
		#ifdef DBG
		kprintf("\nBAD PID ERROR\n");
		#endif
		restore(mask);
		return (pri16)SYSERR;
	}
	prptr = &proctab[pid];
	if (prptr->prstate != PR_SUSP) {
		restore(mask);
		return (pri16)SYSERR;
	}
	prio = prptr->prprio;		/* Record priority to return	*/
	ready(pid);
	if(prio==USERPROCESSPRIO){
		find_pid_in_list(pid)->elig=ELIGIBLE;
	}
#ifdef DBG
		kprintf("\nResume success for PID %d\n", pid);
#endif
	restore(mask);
	return prio;
}
