/* receive.c - receive */

#include "../include/xinu.h"

#define DBG

/*------------------------------------------------------------------------
 *  receive  -  Wait for a message and return the message to the caller
 *------------------------------------------------------------------------
 */
umsg32	receive(void)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	umsg32	msg;			/* Message to return		*/
#ifdef DBG
	kprintf("Receiving message for process %d\n", currpid);
#endif
	mask = disable();
	prptr = &proctab[currpid];
	if (prptr->prhasmsg == FALSE) {
#ifdef DBG
		kprintf("Waiting on msg, blocking\n", currpid);
#endif
		prptr->prstate = PR_RECV;
		resched();		/* Block until message arrives	*/
	}
	msg = prptr->prmsg;		/* Retrieve message		*/
	prptr->prhasmsg = FALSE;	/* Reset message flag		*/
	restore(mask);
	return msg;
}
