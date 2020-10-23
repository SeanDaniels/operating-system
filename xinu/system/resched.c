/* resched.c - resched, resched_cntl */

#include "../include/xinu.h"

struct defer Defer;

uint32 get_timestamp() { return ctr1000; }
//#define RESCHED_CNTL_DBG
//#define RECEIVE_DBG

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
void resched(void) /* Assumes interrupts are disabled	*/
{
  struct procent *ptold; /* Ptr to table entry for old process	*/
  struct procent *ptnew; /* Ptr to table entry for new process	*/

  /* If rescheduling is deferred, record attempt and return */

  if (Defer.ndefers > 0) {
    Defer.attempt = TRUE;
    return;
  }

  /* Point to process table entry for the current (old) process */

  ptold = &proctab[currpid];

  if (ptold->prstate == PR_CURR) { /* Process remains eligible */
    if (ptold->prprio > firstkey(readylist)) {
      ptold->runtime += (ctr1000 - ptold->run_start);
      ptold->run_start = ctr1000;
      return;
    }

    /* Old process will no longer remain current */
    ptold->runtime += ctr1000 - ptold->run_start;
    ptold->prstate = PR_READY;
    insert(currpid, readylist, ptold->prprio);
  }

  /* Force context switch to highest priority ready process */

  currpid = dequeue(readylist);
  ptnew = &proctab[currpid];
  ptnew->prstate = PR_CURR;
  ptnew->run_start = ctr1000;
  preempt = QUANTUM; /* Reset time slice for process	*/
  ctxsw(&ptold->prstkptr, &ptnew->prstkptr);

  /* Old process returns here when resumed */

  return;
}

/*------------------------------------------------------------------------
 *  resched_cntl  -  Control whether rescheduling is deferred or allowed
 *------------------------------------------------------------------------
 */
status resched_cntl(            /* Assumes interrupts are disabled	*/
                    int32 defer /* Either DEFER_START or DEFER_STOP	*/
) {
#ifdef RESCHED_CNTL_DBG
  kprintf("Process %d in resched_cntl\n", currpid);
#endif
  switch (defer) {

  case DEFER_START: /* Handle a deferral request */

    if (Defer.ndefers++ == 0) {
      Defer.attempt = FALSE;
    }
    return OK;

  case DEFER_STOP: /* Handle end of deferral */
    if (Defer.ndefers <= 0) {
      return SYSERR;
    }
    if ((--Defer.ndefers == 0) && Defer.attempt) {
      resched();
    }
    return OK;

  default:
    return SYSERR;
  }
}
