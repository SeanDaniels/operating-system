#include "../include/xinu.h"
#include <stdio.h>
#include <string.h>

#ifdef WORKING
/*------------------------------------------------------------------------
 * xsh_ps - shell command to print the process table
 *------------------------------------------------------------------------
 */
shellcmd xsh_ps(int nargs, char *args[])
{
	struct	procent	*prptr;		/* pointer to process		*/
	/*  hold current time */
	int32	i;			/* index into proctabl		*/
	char *pstate[]	= {		/* names for process states	*/
		"free ", "curr ", "ready", "recv ", "sleep", "susp ",
		"wait ", "rtime"};

	uint32 currentTime = clktime*1000 + ctr1000;
	printf("System has been running for %d seconds\n", currentTime);
	/* For argument '--help', emit help about the 'ps' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tDisplays information about stack trace\n");
		printf("Options:\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}

    if(nargs == 2 && strncmp())

	/* Check for valid number of arguments */

	if (nargs > 1) {
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}

	/* Print header for items from the process table */

	printf("%3s %-16s %5s %4s %4s %10s %-10s %10s %10s\n",
		   "Pid", "Name", "State", "Prio", "Ppid", "Stack Base",
		   "Stack Ptr", "Stack Size", "Thread Time (ms)");

	printf("%3s %-16s %5s %4s %4s %10s %-10s %10s %10s\n",
		   "---", "----------------", "-----", "----", "----",
		   "----------", "----------", "----------", "----------");

	/* Output information for each process */
	uint32 threadRunTime;
	uint32 threadStartTime;
	for (i = 0; i < NPROC; i++) {
		prptr = &proctab[i];
		if (prptr->prstate == PR_FREE) {  /* skip unused slots	*/
			continue;
		}
		threadRunTime = currentTime - prptr->prStartTime;
		printf("%3d %-16s %s %4d %4d 0x%08X 0x%08X %8d %8d\n",
			i, prptr->prname, pstate[(int)prptr->prstate],
			prptr->prprio, prptr->prparent, prptr->prstkbase,
			prptr->prstkptr, prptr->prstklen, threadRunTime);
	}
	return 0;
}
#endif
