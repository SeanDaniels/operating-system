/* resched.c - resched, resched_cntl */

#include "../include/xinu.h"
#define DBG
#define DEBUG_CTXSW
#define MEDIUM_PRIORITY 5
struct	defer	Defer;

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
void	resched(void)		/* Assumes interrupts are disabled	*/
{
    struct procent *ptold;	/* Ptr to table entry for old process	*/
    struct procent *ptnew;	/* Ptr to table entry for new process	*/
    pid32 old_pid, new_pid, lottery_result;
    uint32 test_key;
    /* If rescheduling is deferred, record attempt and return */

    if (Defer.ndefers > 0) {
        Defer.attempt = TRUE;
        return;
    }
    /* Point to process table entry for the current (old) process */

    ptold = &proctab[currpid];
    /*  beginning of current process logic */
    if (ptold->prstate == PR_CURR) {  /* Process remains eligible */
        // current process has highest priority (user processes will have higher priority than nullproc)
        if (ptold->prprio > firstkey(readylist)) {
            ptold->runtime+=(clktime*1000)+ctr1000 - ptold->runtimeStart;
            ptold->runtimeStart=(clktime*1000)+ctr1000;
            if(ptold->prprio==USERPROCESSPRIO){
                qInfo[currpid].runtime=ptold->runtime;
            }
            /*  Allow system process to continue running except for NULLPROC*/
            if(ptold->prprio>USERPROCESSPRIO){
                return;
            }
        }
        /*Current process has highest priority and is user process*/
        if(ptold->prprio==USERPROCESSPRIO){
            /*  Procees has exceeded its allotment */
            if(ptold->runtime>TIME_ALLOTMENT){
                kprintf("***HPQ:lowering priority of process %d***\n", currpid);
                getitem(currpid);
                ptold->prstate = PR_READY;
                ptold->runtime+=(clktime*1000)+ctr1000 - ptold->runtimeStart;
                qInfo[currpid].runtime=0;
                insert(currpid, q_med, ptold->prprio);
            }
            /*  Hasn't exceeded its allotment
             * Schedule normally */
            else{
                ptold->prstate = PR_READY;
                ptold->runtime+=(clktime*1000)+ctr1000 - ptold->runtimeStart;
                insert(currpid,q_high,ptold->prprio);
            }
        }
        /*  Re reinsert system process into system process q */
        else{
            ptold->prstate = PR_READY;
            insert(currpid, readylist, ptold->prprio);
        }
    }
    /*  end of current process logic */
    old_pid = currpid;
    test_key = firstkey(readylist);
    /*  Entry in ready list is NULLPROC
     *  Check user process q's */
    if(test_key<USERPROCESSPRIO){
        //if high priority queue isn't empty
        if(!isempty(q_high)) {
        #ifdef DBG
            kprintf("HPQ DQ attempt\n");
        #endif
            currpid = dequeue(q_high);
        #ifdef DBG
            kprintf("Getting pid from HPQ, PID: %d\n", currpid);
        #endif
        }
        else{
            currpid = dequeue(readylist);
        }
    }
    /*  Entry in ready list a system process */
    else{
        currpid = dequeue(readylist);
        #ifdef DBG
            kprintf("PID: %d\n", currpid);
        #endif
    }
    ptnew = &proctab[currpid];
    new_pid = currpid;
    ptnew->runtimeStart = (clktime*1000)+ctr1000;
    ptnew->prstate = PR_CURR;
    if(new_pid!=old_pid){
        ptnew->num_ctxsw+=1;
#ifdef DEBUG_CTXSW
        kprintf("\nctxsw::%d-%d\n", old_pid,new_pid);
#endif
    }
    preempt = QUANTUM;		/* Reset time slice for process	*/
    ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
    /* Old process returns here when resumed */
    return;
}

void med_resched(void){
    struct procent *ptold;	/* Ptr to table entry for old process	*/
    struct procent *ptnew;	/* Ptr to table entry for new process	*/
    pid32 old_pid, new_pid;
    uint32 test_key, test_runtime;
    #ifdef DBG
    kprintf("***Rescheduling medium priority queue***\n");
    #endif
    //ready system process
    if(firstkey(readylist)>USERPROCESSPRIO){
#ifdef DBG
        kprintf("***Ready list has higher priority task waiting***\n");
#endif
        med_preempt = 2*QUANTUM;
        return;
    }
    //ready process in higher queue
    if(!isempty(q_high)){
#ifdef DBG
        kprintf("HPQ has higher priority task waiting\n");
#endif
        med_preempt = 2*QUANTUM;
        return;
    }
    /* If rescheduling is deferred, record attempt and return */
    if (Defer.ndefers > 0) {
        Defer.attempt = TRUE;
        return;
    }
    /* Point to process table entry for the current (old) process */
    
    ptold = &proctab[currpid];
    /*  beginning of current process logic */
    if (ptold->prstate == PR_CURR) {  /* Process remains eligible */
        // current process has highest priority of the medium q
        if (ptold->prprio > firstkey(q_med)) {
            ptold->runtime+=(clktime*1000)+ctr1000 - ptold->runtimeStart;
            qInfo[currpid].runtime+=ptold->runtime;    
            ptold->runtimeStart=(clktime*1000)+ctr1000;
            return;
        }
        /*Current process has highest priority and is user process*/

        /*  Procees has exceeded its allotment */
        kprintf("In  q_med\nPID %d Current process runtime %d\n", currpid, qInfo[currpid].runtime);
        if(ptold->prprio==USERPROCESSPRIO){
            if(qInfo[currpid].runtime>TIME_ALLOTMENT){
#ifdef DBG
                kprintf("In q_med\nLower priority\nProcess runtime in this q: %d\n", currpid, qInfo[currpid].runtime);
#endif
                getitem(currpid);
                ptold->prstate = PR_READY;
                ptold->runtime+=(clktime*1000)+ctr1000 - ptold->runtimeStart;
                qInfo[currpid].runtime=0;
                insert(currpid, q_low, ptold->prprio);
            }   
        }
            /*  Hasn't exceeded its allotment
             * Schedule normally */
        else{
            ptold->prstate = PR_READY;
            ptold->runtime+=(clktime*1000)+ctr1000 - ptold->runtimeStart;
            qInfo[currpid].runtime = ptold->runtime;
            insert(currpid,q_med,ptold->prprio);
        }
    }
    /*  Re reinsert userprocess into user process q */
    else{
        if(ptold->prprio!=USERPROCESSPRIO){
           insert(currpid, readylist, ptold->prprio);
        }
        else{
            insert(currpid, q_med, ptold->prprio);
        }
    }
   
    /*  end of current process logic */

    old_pid = currpid;
    currpid = dequeue(q_med);
    if(currpid==-1){
#ifdef DBG
        kprintf("Guys we got a problem\n");
#endif
        currpid = 0;
    }
    ptnew = &proctab[currpid];
    new_pid = currpid;
    ptnew->runtimeStart = (clktime*1000)+ctr1000;
    ptnew->prstate = PR_CURR;
    if(new_pid!=old_pid){
        ptnew->num_ctxsw+=1;
#ifdef DEBUG_CTXSW
        kprintf("\nctxsw::%d-%d\n", old_pid,new_pid);
#endif
    }
    med_preempt = 2*QUANTUM;		/* Reset time slice for process	*/
    ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
    /* Old process returns here when resumed */
    return;
}

/*------------------------------------------------------------------------
 *  resched_cntl  -  Control whether rescheduling is deferred or allowed
 *------------------------------------------------------------------------
 */
status	resched_cntl(		/* Assumes interrupts are disabled	*/
    int32	defer		/* Either DEFER_START or DEFER_STOP	*/
)
{
    switch (defer) {

        case DEFER_START:	/* Handle a deferral request */

            if (Defer.ndefers++ == 0) {
                Defer.attempt = FALSE;
            }
            return OK;

        case DEFER_STOP:	/* Handle end of deferral */
            if (Defer.ndefers <= 0) {
                return SYSERR;
            }
            if ( (--Defer.ndefers == 0) && Defer.attempt ) {
                resched();
            }
            return OK;

        default:
            return SYSERR;
    }
}
