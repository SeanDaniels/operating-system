
#include "../include/xinu.h"

//#define STACKCOPYONE
//#define STACKCOPYTWODBG
#define DBG
#define DBGPRINT

local pid32 newpid(void);
uint32 *childStackAddr;


unsigned long* stack_copy(unsigned long* framePointer){
    unsigned long *temp = NULL;
    uint32 savePointer;
    //found sm
    if(*framePointer==STACKMAGIC)
        return framePointer;
    // temp will equal a pointer to the previous frame pointer 
    temp = stack_copy(*framePointer);

#ifdef STACKCOPYTWODBG
    kprintf("\nCopy from %X (%X) to %X (%X)\nStarting at child stack address (%X)\n", temp, *temp, framePointer, *framePointer, childStackAddr);
#endif
    //save framepointer location
    if(*temp==STACKMAGIC){
        savePointer = (uint32)childStackAddr;
    }
    else{
        savePointer = (uint32)childStackAddr+4;
    }
#ifdef STACKCOPYTWODBG
    kprintf("Saving pointer to %X\n", savePointer);
    kprintf("Child stack address: %X\n", childStackAddr);
    kprintf("Parent stack ----- Child stack\n");
#endif
    //iterate and copy until next fp
    while(temp>framePointer){
        //copy from parentStack[x] to childStack[x]
        *childStackAddr=*temp;
#ifdef STACKCOPYTWODBG
        kprintf("(%X) %X ----- (%X) %X\n", temp, *temp, childStackAddr, *childStackAddr);
#endif 
        childStackAddr--;
        temp--;
    }
    //print next fp
#ifdef STACKCOPYTWODBG
    kprintf("\nAt next FP: (%X) %X\n", temp, *temp);
    kprintf("\nSave child stack frame pointer (%X) into child stack address (%X)\n", savePointer, childStackAddr);
#endif 
    //save 'savePointer' into current child stack address
 
    *childStackAddr=savePointer;
#ifdef STACKCOPYTWODBG
    kprintf("New child stack entry:\n(%X) %X\n", childStackAddr, *childStackAddr);
    kprintf("Returning updated framepointer:(%X) %X\n", 1, *(framePointer-1));
#endif
    childStackAddr--;
    return(framePointer-1);
}

pid32 fork(){
    uint32  savsp, *pushsp;
    intmask mask;    	/* Interrupt mask		*/
    pid32 childProcessPID, parentProcessPID;		/* Stores new process id	*/
    struct	procent *childProcessPtr, *parentProcessPtr;		/* Pointer to proc. table entry */
    uint32 *parentStackAddr;		/* Stack address		*/
    int32 i;
    uint32 *a, startTimeMS;		/* Points to list of args	*/
    uint32 parentStackSize, childStackSize, valueAtParentStackAddress, stopCopyAddr, endOfNewStack;
    char* parentName[PNMLEN];
    unsigned long *parentSP, *stackIterator, *currentSP, *ebp, *currentFP, *startCopy, *topStackFramePointer, *parentReturnAddr, *frameGrabber, *ogChildStackBaseAddr;

    parentProcessPID = currpid;
    /*get stack pointer and frame pointer of parent process*/
    asm("movl %%ebp, %0\n" :"=r"(ebp)); //stop copy point
    asm("movl %%esp, %0\n" :"=r"(currentSP)); //stop copy point
    /* kprintf("Stack Trace inside of fork\n"); */
    /* stacktrace(currpid); */
    /* kprintf("End of trace\n"); */
    currentFP = ebp;
    topStackFramePointer = ebp;
    parentReturnAddr = ebp - 4;
    startTimeMS = clktime * 1000;

    mask = disable();
    // get current process' process entry from PCB
    parentProcessPtr = &proctab[currpid];
    // I want the address
    startCopy = (unsigned long *)parentProcessPtr->prstkbase;
    parentStackSize = parentProcessPtr->prstklen;
    /* get parent name */
    *parentName = parentProcessPtr->prname;


    /*check if process space exists, stack space exists */
    if ( ((childProcessPID=newpid()) == SYSERR) ||
         ((childStackAddr = (uint32 *)getstk(parentStackSize)) == (uint32 *)SYSERR) ) {
        restore(mask);
        return SYSERR;
    }
    prcount++;
    childProcessPtr = &proctab[childProcessPID];
    //set time of new process creation
    childProcessPtr->prStartTime = startTimeMS + ctr1000;
    /* Initialize process table entry for new process */
    childProcessPtr->prstate = PR_READY;	/* Initial state is suspended	*/
    childProcessPtr->prprio = parentProcessPtr->prprio;
    childProcessPtr->prstkbase = (char *)childStackAddr;
    ogChildStackBaseAddr = (unsigned long*)childProcessPtr->prstkbase;
    childProcessPtr->prstklen = parentStackSize;
    /*set name*/
    childProcessPtr->prname[PNMLEN-1] = NULLCH;
    /*copy name*/
    for (i=0 ; i<PNMLEN-1 && (childProcessPtr->prname[i]=*parentName[i])!=NULLCH; i++)
        ;

    childProcessPtr->prsem = -1;
    /*  set new process parent to current process */
    childProcessPtr->prparent = parentProcessPID;
    childProcessPtr->prhasmsg = FALSE;
    /* Set up stdin, stdout, and stderr descriptors for the shell	*/
    childProcessPtr->prdesc[0] = CONSOLE;
    childProcessPtr->prdesc[1] = CONSOLE;
    childProcessPtr->prdesc[2] = CONSOLE;

    unsigned long* someValue = stack_copy(ebp);
    unsigned long* bottomOfStack = childStackAddr+1;
#ifdef STACKCOPYTWODBG
    kprintf("\nChild process stack base: %X\n'someValue': %X\n'bottomOfStack' (child stack addr + 1): %X\n", ogChildStackBaseAddr, someValue, bottomOfStack); 
    kprintf("Current child stack addr: %X\n", childStackAddr);
    kprintf("Stack content before updating register values\n");
    for(; ogChildStackBaseAddr>=bottomOfStack; ogChildStackBaseAddr--){
        kprintf("(%X) %X\n", ogChildStackBaseAddr, *ogChildStackBaseAddr);
    }
#endif
    savsp = (uint32)bottomOfStack;
    *childStackAddr = 0x00000200;
    *--childStackAddr = NPROC;			/* %eax accumulator*/
    *--childStackAddr = 0;			/* %ecx counter register*/
    *--childStackAddr = 0;			/* %edx  Data register*/
    *--childStackAddr = parentReturnAddr;			/* %ebx base register*/
    *--childStackAddr = 0;			/* %esp stack pointer; value filled in below	*/
    pushsp = childStackAddr;
    *--childStackAddr = savsp;		/* %ebp (while finishing ctxsw) base pointer	*/
    *--childStackAddr = 0;			/* %esi source index*/
    *--childStackAddr = 0;			/* %edi destination index*/
   #ifdef STACKCOPYTWODBG 
    kprintf("Stack content after updating register values (using child stack base address as pointer)\n");
    for(; ogChildStackBaseAddr>=childStackAddr; ogChildStackBaseAddr--){
        kprintf("(%X) %X\n", ogChildStackBaseAddr, *ogChildStackBaseAddr);
    }
    #endif
    *pushsp = (unsigned long) (childProcessPtr->prstkptr=(char*)childStackAddr);



    insert(childProcessPID, readylist, childProcessPtr->prprio);
    restore(mask);
    /*if everything works      */
    return childProcessPID;
}

local pid32 newpid(void)
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
