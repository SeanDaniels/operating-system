#include "../include/xinu.h"

void print_proc(){
    struct	procent	*prptr;		/* pointer to process		*/
    uint32 threadRunTime;
    uint32 threadStartTime;
    uint32 i;
    uint32 currentTime = clktime*1000 + ctr1000;
    char *pstate[]	= {		/* names for process states	*/
    "free ", "curr ", "ready", "recv ", "sleep", "susp ",
    "wait ", "rtime"};

    printf("%3s %-16s %5s %4s %4s %10s %-10s %10s %10s %10s\n",
           "Pid", "Name", "State", "Prio", "Ppid", "Stack Base",
           "Stack Ptr", "Stack Size", "Thread Time(ms)", "Thread Runtime(ms)");

    printf("%3s %-16s %5s %4s %4s %10s %-10s %10s %10s %10s\n",
           "---", "----------------", "-----", "----", "----",
           "----------", "----------", "----------", "---------------", "---------------");

    /* Output information for each process */
    for (i = 0; i < NPROC; i++) {
        prptr = &proctab[i];
        if (prptr->prstate == PR_FREE) {  /* skip unused slots	*/
            continue;
        }
        threadRunTime = currentTime - prptr->prStartTime;
        printf("%3d %-16s %s %4d %4d 0x%08X 0x%08X %6d %15d %15d\n",
               i, prptr->prname, pstate[(int)prptr->prstate],
               prptr->prprio, prptr->prparent, prptr->prstkbase,
               prptr->prstkptr, prptr->prstklen, threadRunTime, prptr->runtime);
    }
    printf("\n%3s %-16s %5s %4s %4s %10s %-10s %10s %10s %10s %10s\n", "PID", "Switch", "    ","    ","    ","    ","    ","    ","    ","    ", "  ");
    printf("%3s %-16s %5s %4s %4s %10s %-10s %10s %10s %10s\n",
           "---", "----------------", "-----", "----", "----",
           "----------", "----------", "----------", "---------------", "---------------");
    for (i = 0; i < NPROC; i++) {
        prptr = &proctab[i];
        if (prptr->prstate == PR_FREE) {  /* skip unused slots	*/
            continue;
        }
        printf("%3d %5d\n", i, prptr->num_ctxsw);
    }

    /*  show ready list   */
#ifdef DBG
    print_ready_list();
#endif
    return;

}
