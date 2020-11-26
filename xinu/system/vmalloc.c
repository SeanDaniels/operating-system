#include "../include/xinu.h"
#define DBG
#define TABLE_MASK 0x000000FF
extern struct memblk ptlist;
extern uint32 process_virtual_pages[NPROC];

/* Size of memory requested	*/
char *vmalloc(uint32 nbytes)
{
    uint32 i, firstAvailableEntry, pageTableEntriesNeeded, pageDirectoryEntriesNeeded = nbytes/PAGE_SIZE;
    pageTableEntriesNeeded = pageDirectoryEntriesNeeded*PTE_PER_PAGE;
    char* newPageStartPoint;
    unsigned long pdbr, newPDEntry, endPoint, pointerReturn;
    intmask mask;
    mask = disable();
    pdbr = proctab[currpid].pdbr;
    pd_t *userProcessBaseDir = (pd_t *)(pdbr);
    pt_t *newPageTable;
#ifdef DBG
        kprintf("Allocating %d pages for process %d\n", pageDirectoryEntriesNeeded, currpid);
#endif
    firstAvailableEntry = check_page_directory(currpid);
    if(!firstAvailableEntry){
        restore(mask);
        return (char*)0;
    }
    else{
      /* get block for user process page */
        newPageStartPoint = getblk(&ptlist, nbytes);

        newPageTable = (pt_t *)newPageStartPoint;
        /* Set user process directory to point to this new chunk of memory */
        newPDEntry = (unsigned long)newPageStartPoint >> 12;
        /* Update process page directory */
        /* Set entries in page directory with correct base address and avialability*/
        update_page_directory(pdbr, newPDEntry, firstAvailableEntry, pageDirectoryEntriesNeeded);
        /* Update process page table*/
        update_page_table((unsigned long)newPageStartPoint, pageTableEntriesNeeded);
        pointerReturn = PT_START | ((newPDEntry - (pdbr >> 12)) - 1) << 12;
#ifdef DBG
        kprintf("Virtual Start Point: 0x%08X\n", PT_START);
        kprintf("Entry in user process page directory: 0x%08X\n",userProcessBaseDir[firstAvailableEntry].pd_base);
        kprintf("Shifted PDBR: 0x%08X\n", pdbr >> 12);
        kprintf("Result of subtracing new page table start point and process pdbr: 0x%08X\n",pointerReturn);
#endif
    }
    restore(mask);
    return (char *)pointerReturn;
}

uint32 check_page_directory(pid32 pid) {
  uint32 i;
  pd_t *userProcessBaseDir = (pd_t *)(proctab[pid].pdbr);
#ifdef DBG
#endif
  for (i = 0; i < PTE_PER_PAGE; i++) {
    if (userProcessBaseDir[i].pd_avail == 1) {
      return i;
    }
  }
  return 0;
}

void update_page_directory(unsigned long processPDBR, unsigned long pdBaseStart, uint32 index, uint32 pages) {
  uint32 i, end, j = 0;
  end = index + pages;
  pd_t *pde = (pd_t *)processPDBR;
  for (i = index; i < end; i++){
      #ifdef DBG
      kprintf("Updating Page Directory with value 0x%08X\n", pdBaseStart+j);
      #endif
      pde[i].pd_base = pdBaseStart + j++;
      pde[i].pd_avail = 0;
      pde[i].pd_alloc = 0;
  }
  return;
}

void update_page_table(unsigned long pageTableStartAddress, uint32 pageTableEntries){
    uint32 i;
    pt_t *pte = (pt_t *)((char*)pageTableStartAddress);
    for(i = 0; i < pageTableEntries; i++){
        pte[i].pt_avail = 0;
    }

}

syscall vfree(char* pointer, uint32 nbytes){
    pid32 pid = currpid;
    uint32 i, virtualPageCount = allocated_virtual_pages(pid);
    unsigned long vHeapStart, pdbr = proctab[pid].pdbr;
    pd_t *pde = (pd_t*)pdbr;
    if(nbytes>(virtualPageCount*PAGE_SIZE)) return SYSERR;
    #ifdef DBG
    kprintf("Current PID: %d\n", pid);
    kprintf("PID's pdbr: 0x%08X\n", pdbr);
    kprintf("Passed pointer: 0x%08X\n", pointer);
    kprintf("Number of virtual pages: %d\n", virtualPageCount);
    #endif
    return OK;
}
