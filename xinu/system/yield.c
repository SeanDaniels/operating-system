/* yield.c - yield */

#include "../include/xinu.h"

/*------------------------------------------------------------------------
 *  yield  -  Voluntarily relinquish the CPU (end a timeslice)
 *------------------------------------------------------------------------
 */
syscall	yield(void)
{
	intmask	mask;			/* Saved interrupt mask		*/
	mask = disable();
	resched();
	restore(mask);
	return OK;
}
