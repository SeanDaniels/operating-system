#include "../include/xinu.h"
//#define DBG
void burst_execution(uint32 number_bursts, uint32 burst_duration, uint32 sleep_duration){
	struct procent *ptcurr;	/* Ptr to table entry for old process	*/
    uint32 burstEnd;
    uint32 currTime;
    uint32 startTime;
	ptcurr = &proctab[currpid];
    for(uint32 i = 0; i < number_bursts; i++){
        #ifdef DBG
        kprintf("Burst duration: %d\n", burst_duration);
        #endif
        burstEnd = clktime*1000+ctr1000+burst_duration;
        currTime = clktime*1000+ctr1000;
        #ifdef DBG
        kprintf("Process ID: %d\n", currpid);
        kprintf("Current time: %d, Burst end time: %d\n", currTime, burstEnd);
        #endif
        while (currTime<burstEnd){
            currTime = clktime*1000+ctr1000;
        }
        #ifdef DBG
        currTime = clktime*1000+ctr1000;
        kprintf("Burst ending. Current time: %d\n", currTime);
        kprintf("Going to sleep (iteration %d)\n", i);
        #endif
        sleepms(sleep_duration);
    }
    currTime = clktime*1000+ctr1000;
    startTime = ptcurr->prStartTime;
    #ifdef DBG
    kprintf("(%d)Job start time: %d, Job complete time: %d, Turnaround time: %d\n", currpid,startTime, currTime, currTime - startTime);
    #endif
    sleepms(sleep_duration);
    return;
}

