/* stacktrace.c - stacktrace */
#include "../include/xinu.h"

#define STKDETAIL

/*------------------------------------------------------------------------
 * stacktrace - print a stack backtrace for a process
 *------------------------------------------------------------------------
 */
syscall stacktrace(int pid)
{
	struct procent	*proc = &proctab[pid];
	unsigned long	*sp, *fp;

	if (pid != 0 && isbadpid(pid))
		return SYSERR;

	if (pid == currpid) {
		asm("movl %%esp, %0\n" :"=r"(sp));
		asm("movl %%ebp, %0\n" :"=r"(fp));
	}
	else {
		sp = (unsigned long *)proc->prstkptr;
		fp = sp + 2; 		/* where ctxsw leaves it */
	}
	kprintf("sp %X fp %X proc->prstkbase %X\n", sp, fp, proc->prstkbase);
#ifdef STKDETAIL
	/*  while stack pointer is less than current requested pids stk base
	 * Checking the condition means that the stack pointer is less than the pids stack pointer */
	while (sp < (unsigned long *)proc->prstkbase) {
		/*increment stack pointer*/
		/*Move up the stack*/
		/*Frame pointer is closer to the top than the stack pointer*/
		for (; sp < fp; sp++)
			kprintf("DATA (%08X) %08X (%d)\n", sp, *sp, *sp);
		if (*sp == STACKMAGIC){
			break;
		}
		kprintf("\nFP   (%08X) %08X (%d)\n", sp, *sp, *sp);
		//set frame pointer to value that current frame pointer points to
		fp = (unsigned long *) *sp++;
		/*  frame pointer is less than stack pointer */
		if (fp <= sp) {
			kprintf("bad stack, fp (%08X) <= sp (%08X)\n", fp, sp);
			return SYSERR;
		}
		kprintf("(ADDR:%08X) RET  0x%X\n", sp, *sp);
		sp++;
	}
	kprintf("STACKMAGIC (should be %X): %X\n", STACKMAGIC, *sp);
	if (sp != (unsigned long *)proc->prstkbase) {
		kprintf("unexpected short stack\n");
		return SYSERR;
	}
#endif
	return OK;
}
