/* queue.c - enqueue, dequeue */

#include "../include/xinu.h"
struct qentry	queuetab[NQENT];	/* Table of process queues	*/
struct qEntryInfo qInfo[NPROC];

/*------------------------------------------------------------------------
 *  enqueue  -  Insert a process at the tail of a queue
 *------------------------------------------------------------------------
 */
pid32	enqueue(
	pid32		pid,		/* ID of process to insert	*/
	qid16		q		/* ID of queue to use		*/
)
{
	qid16	tail, prev;		/* Tail & previous node indexes	*/

	if (isbadqid(q) || isbadpid(pid)) {
		return SYSERR;
	}

	tail = queuetail(q);
	prev = queuetab[tail].qprev;

	queuetab[pid].qnext  = tail;	/* Insert just before tail node	*/
	queuetab[pid].qprev  = prev;
	queuetab[prev].qnext = pid;
	queuetab[tail].qprev = pid;
	return pid;
}

/*------------------------------------------------------------------------
 *  dequeue  -  Remove and return the first process on a list
 *------------------------------------------------------------------------
 */
pid32	dequeue(
	qid16		q		/* ID of queue to use		*/
)
{
	pid32	pid;			/* ID of process removed	*/

	if (isbadqid(q)) {
		return SYSERR;
	} else if (isempty(q)) {
		return EMPTY;
	}

	pid = getfirst(q);
	queuetab[pid].qprev = EMPTY;
	queuetab[pid].qnext = EMPTY;
	return pid;
}

void print_queue(qid16 q){
	pid32 pid;
	qid16 tail;


	if (isbadqid(q)) {
		kprintf("BAD PID\n");
		return;
	} else if (isempty(q)) {
		kprintf("EMPTY Q\n");
		return;
	}
	tail = queuetail(q);
	pid = getfirst(q);
	while(pid!=tail){
		kprintf("PID: %d\n", pid);
		pid = queuetab[pid].qnext;
	}
	kprintf("All entries printed\n");
	return;

}
