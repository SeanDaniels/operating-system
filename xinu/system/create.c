/* create.c - create, newpid */

#include "../include/xinu.h"
#include <stdarg.h>
//#define DBG
local	int newpid();

//#define BURST
//#define TIMED
//#define BUILTIN
/*------------------------------------------------------------------------
 *  create  -  Create a process to start running a function on x86
 *------------------------------------------------------------------------
 */
pid32	create(
	  void *funcaddr,	/* Address of the function	*/
 	  uint32 ssize,		/* Stack size in bytes		*/
	  pri16 priority,	/* Process priority > 0		*/
	  char *name,		/* Name (for debugging)		*/
	  uint32 nargs,		/* Number of args that follow	*/
	  ...
	)
{
	uint32 savsp, *pushsp;
	intmask mask;    	/* Interrupt mask		*/
	pid32 pid;		/* Stores new process id	*/
	struct	procent	*prptr;		/* Pointer to proc. table entry */
	int32 i;
	uint32 *a;		/* Points to list of args	*/
	uint32 *saddr;		/* Stack address		*/
	uint32 startTimeMS;

	startTimeMS = clktime * 1000;

	mask = disable();
	if (ssize < MINSTK)
		ssize = MINSTK;

	ssize = (uint32) roundmb(ssize);
	if ( (priority < 1) || ((pid=newpid()) == SYSERR) ||
	     ((saddr = (uint32 *)getstk(ssize)) == (uint32 *)SYSERR) ) {
		restore(mask);
		return SYSERR;
	}

	prcount++;
	prptr = &proctab[pid];

	/* Initialize process table entry for new process */
	prptr->prstate = PR_SUSP;	/* Initial state is suspended	*/
	prptr->prprio = priority;
	prptr->prstkbase = (char *)saddr;
	prptr->prstklen = ssize;
	prptr->prname[PNMLEN-1] = NULLCH;
	/*  Set process start time */
	prptr->prStartTime = startTimeMS + ctr1000;
	for (i=0 ; i<PNMLEN-1 && (prptr->prname[i]=name[i])!=NULLCH; i++)
		;
	prptr->prsem = -1;
	prptr->prparent = (pid32)getpid();
	prptr->prhasmsg = FALSE;

	/* Set up stdin, stdout, and stderr descriptors for the shell	*/
	prptr->prdesc[0] = CONSOLE;
	prptr->prdesc[1] = CONSOLE;
	prptr->prdesc[2] = CONSOLE;

	prptr->runtime = 0;
	prptr->runtimeStart = 0;
	prptr->runtimePause = 0;
	prptr->runtimeFinish = 0;
	prptr->num_ctxsw = 0;
	/* Initialize stack as if the process was called		*/

	*saddr = STACKMAGIC;
	savsp = (uint32)saddr;

	/* Push arguments */
	a = (uint32 *)(&nargs + 1);	/* Start of args		*/

	a += nargs -1;			/* Last argument		*/

	for ( ; nargs > 0 ; nargs--)	/* Machine dependent; copy args	*/
		*--saddr = *a--;	 /* onto created process's stack	*/

	*--saddr = (long)INITRET;	/* Push on return address	*/

	/* The following entries on the stack must match what ctxsw	*/
	/*   expects a saved process state to contain: ret address,	*/
	/*   ebp, interrupt mask, flags, registers, and an old SP	*/

	*--saddr = (long)funcaddr;	/* Make the stack look like it's*/
					/*   half-way through a call to	*/
					/*   ctxsw that "returns" to the*/
					/*   new process		*/
	*--saddr = savsp;		/* This will be register ebp	*/
					/*   for process exit		*/
	savsp = (uint32) saddr;		/* Start of frame for ctxsw	*/
	*--saddr = 0x00000200;		/* New process runs with	*/
					/*   interrupts enabled		*/

	/* Basically, the following emulates an x86 "pushal" instruction*/

	*--saddr = 0;			/* %eax accumulator*/
	*--saddr = 0;			/* %ecx counter register*/
	*--saddr = 0;			/* %edx  Data register*/
	*--saddr = 0;			/* %ebx base register*/
	*--saddr = 0;			/* %esp stack pointer; value filled in below	*/
	pushsp = saddr;			/* Remember this location *save stack pointer*	*/
	*--saddr = savsp;		/* %ebp (while finishing ctxsw) base pointer	*/
	*--saddr = 0;			/* %esi source index*/
	*--saddr = 0;			/* %edi destination index*/
	*pushsp = (unsigned long) (prptr->prstkptr = (char *)saddr);
	restore(mask);
	return pid;
}

/*------------------------------------------------------------------------
 *  create_user_process -  Explicitly create a user defined process
 *------------------------------------------------------------------------
 */
pid32 create_user_process(void *funcaddr, uint32 ssize, char *name, uint32 nargs, ...){
#ifdef DBG
	kprintf("Creating user process with %d args\n", nargs);
	kprintf("The args are %d, %d, %d\n", *(&nargs+1), *(&nargs+2), *(&nargs+3));
#endif
#ifdef BURST
	return create((void *)funcaddr, ssize, USERPROCESSPRIO, name, nargs, *(&nargs+1), *(&nargs+2), *(&nargs+3));
#endif
#ifdef TIMED
	return create((void *)funcaddr, ssize, USERPROCESSPRIO, name, nargs, *(&nargs+1));
#endif
#ifdef BUILTIN
	void *createArgs = __builtin_apply_args();
	void* returnValue = __builtin_apply((void*)create, createArgs, 200);
	pid32 return_pid = (pid32 *) *(__builtin_return(returnValue));
	kprintf("\n__builtin_returnValue: %d\n", return_pid);
	return -1;
#endif
	uint32 savsp, *pushsp;
	intmask mask;    	/* Interrupt mask		*/
	pid32 pid;		/* Stores new process id	*/
	struct	procent	*prptr;		/* Pointer to proc. table entry */
	int32 i;
	uint32 *a;		/* Points to list of args	*/
	uint32 *saddr;		/* Stack address		*/
	uint32 startTimeMS;
	uint32 priority = USERPROCESSPRIO;

	startTimeMS = clktime * 1000;

	mask = disable();
	if (ssize < MINSTK)
		ssize = MINSTK;

	ssize = (uint32) roundmb(ssize);
	if ( (priority < 1) || ((pid=newpid()) == SYSERR) ||
	     ((saddr = (uint32 *)getstk(ssize)) == (uint32 *)SYSERR) ) {
		restore(mask);
		return SYSERR;
	}

	prcount++;
	prptr = &proctab[pid];

	/* Initialize process table entry for new process */
	prptr->prstate = PR_SUSP;	/* Initial state is suspended	*/
	prptr->prprio = priority;
	prptr->prstkbase = (char *)saddr;
	prptr->prstklen = ssize;
	prptr->prname[PNMLEN-1] = NULLCH;
	/*  Set process start time */
	prptr->prStartTime = startTimeMS + ctr1000;
	for (i=0 ; i<PNMLEN-1 && (prptr->prname[i]=name[i])!=NULLCH; i++)
		;
	prptr->prsem = -1;
	prptr->prparent = (pid32)getpid();
	prptr->prhasmsg = FALSE;

	/* Set up stdin, stdout, and stderr descriptors for the shell	*/
	prptr->prdesc[0] = CONSOLE;
	prptr->prdesc[1] = CONSOLE;
	prptr->prdesc[2] = CONSOLE;

	prptr->runtime = 0;
	prptr->runtimeStart = 0;
	prptr->runtimePause = 0;
	prptr->runtimeFinish = 0;
	prptr->num_ctxsw = 0;
	/* Initialize stack as if the process was called		*/

	*saddr = STACKMAGIC;
	savsp = (uint32)saddr;

	/* Push arguments */
	a = (uint32 *)(&nargs + 1);	/* Start of args		*/

	a += nargs -1;			/* Last argument		*/

	for ( ; nargs > 0 ; nargs--)	/* Machine dependent; copy args	*/
		*--saddr = *a--;	 /* onto created process's stack	*/

	*--saddr = (long)INITRET;	/* Push on return address	*/

	/* The following entries on the stack must match what ctxsw	*/
	/*   expects a saved process state to contain: ret address,	*/
	/*   ebp, interrupt mask, flags, registers, and an old SP	*/

	*--saddr = (long)funcaddr;	/* Make the stack look like it's*/
					/*   half-way through a call to	*/
					/*   ctxsw that "returns" to the*/
					/*   new process		*/
	*--saddr = savsp;		/* This will be register ebp	*/
					/*   for process exit		*/
	savsp = (uint32) saddr;		/* Start of frame for ctxsw	*/
	*--saddr = 0x00000200;		/* New process runs with	*/
					/*   interrupts enabled		*/

	/* Basically, the following emulates an x86 "pushal" instruction*/

	*--saddr = 0;			/* %eax accumulator*/
	*--saddr = 0;			/* %ecx counter register*/
	*--saddr = 0;			/* %edx  Data register*/
	*--saddr = 0;			/* %ebx base register*/
	*--saddr = 0;			/* %esp stack pointer; value filled in below	*/
	pushsp = saddr;			/* Remember this location *save stack pointer*	*/
	*--saddr = savsp;		/* %ebp (while finishing ctxsw) base pointer	*/
	*--saddr = 0;			/* %esi source index*/
	*--saddr = 0;			/* %edi destination index*/
	*pushsp = (unsigned long) (prptr->prstkptr = (char *)saddr);
	restore(mask);
	return pid;
}

/*------------------------------------------------------------------------
 *  newpid  -  Obtain a new (free) process ID
 *------------------------------------------------------------------------
 */
local pid32	newpid(void)
{
	uint32 i;			/* Iterate through all processes*/
	static pid32 nextpid = 1;	/* Position in table to try or	*/
					/*   one beyond end of table	*/

	/* Check all NPROC slots */

	for (i = 0; i < NPROC; i++) {
		nextpid %= NPROC;	/* Wrap around to beginning */
		if (proctab[nextpid].prstate == PR_FREE) {
			return nextpid++;
		} else {
			nextpid++;
		}
	}
	return (pid32) SYSERR;
}
