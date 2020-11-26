#include "../include/xinu.h"

#define DBG

extern struct memblk ffslist;
extern struct memblk ptlist;

uint32 pages_used;
uint32 process_ffs_used[NPROC];
uint32 process_virtual_pages[NPROC];

void initialize_system_pd() {

  unsigned long i, j, pd_end, pt_end, page_count;
#ifdef DBG
  kprintf("Begin page table init\n");
#endif
  page_count = (XINU_PAGES + MAX_PT_SIZE + MAX_FFS_SIZE) / PTE_PER_PAGE;
  char *pd_baseAddress = getblk(&ptlist, PAGE_SIZE);
  char *pt_baseAddress = getblk(&ptlist, page_count * PAGE_SIZE);
  // cast the 32bit pointer to page directory structure
  pd_t *system_pd = (pd_t *)pd_baseAddress;
  // cast the 32bit pointer to page table structure structure
  pt_t *system_pt = (pt_t *)pt_baseAddress;

  // initialize page directory
  // 4KB directory, has data for 1024 individual pages
  // Create a page directory with 25 entries
  clear_page(pd_baseAddress);
#ifdef DBG
  kprintf("System PD Start Directory: 0x%08X\n", pd_baseAddress);
#endif
  for (i = 0; i < page_count; i++) {
    system_pd[i].pd_pres = 1;
    system_pd[i].pd_write = 1;
    system_pd[i].pd_base = (unsigned int)(((unsigned)pt_baseAddress >> 12) + i);
    system_pd[i].pd_alloc = 1;
    system_pd[i].pd_avail = 0;
  }

  // initialize page table
  // 4KB page table, either contains pages or page entries I don't know
  // Created a page table that iterates and initializes ALL system pages
  for (i = 0; i < (XINU_PAGES + MAX_PT_SIZE + MAX_FFS_SIZE); i++) {
    system_pt[i].pt_pres = 1;
    system_pt[i].pt_write = 1;
    system_pt[i].pt_alloc = 1;
    system_pt[i].pt_avail = 0;
    system_pt[i].pt_base = (unsigned int)i;
  }
  /* Initialize process ffs and virtual page info arrays */
  for(i = 0; i < NPROC; i++){
    process_ffs_used[i] = 0;
    process_virtual_pages[i] = XINU_PAGES;
  }

#ifdef DBG
  kprintf("Page table initiallized\n");
#endif
  /* Update CR3 register */
  write_cr3(XINU_PAGES*PAGE_SIZE);
  return;
}

char* initialize_user_pd(pid32 pid){
    uint32 i;
    char* pdAddress;
    pd_t *temp_pd;
    pdAddress = getblk(&ptlist, PAGE_SIZE);
    if(pdAddress == (char *)SYSERR){
        return SYSERR;
    }
    clear_page(pdAddress);
    temp_pd = (pd_t *)pdAddress;
    //initialize new page directory
    for(i = 0; i<(XINU_PAGES)/PTE_PER_PAGE; i++){
      temp_pd[i].pd_pres = 1;
      temp_pd[i].pd_write = 1;
      temp_pd[i].pd_alloc = 1;
      temp_pd[i].pd_avail = 0;
      temp_pd[i].pd_base =
          (unsigned int)(((unsigned)temp_pd >> 12) + i);
    }
#ifdef DBG
//    print_page_dir(pdAddress, pid);
#endif
    return pdAddress;
}

void print_ffs_pages(){
    struct memblk *memptr;
    uint32 free_ffs;
    free_ffs = 0;
    for(memptr = ffslist.mnext; memptr != NULL; memptr = memptr->mnext){
        free_ffs += memptr->mlength;
    }
    kprintf("%10d bytes of FFS\n", free_ffs);
    for (memptr = ffslist.mnext; memptr != NULL; memptr = memptr->mnext) {
      kprintf("[0x%08X to 0x%08X]\n", (uint32)memptr,
              ((uint32)memptr) + memptr->mlength - 1);
    }
    return;
}

uint32 free_ffs_pages(){
    struct memblk *memptr;
    uint32 free_ffs;
    free_ffs = 0;
    for(memptr = ffslist.mnext; memptr != NULL; memptr = memptr->mnext){
        free_ffs += memptr->mlength;
    }
    return free_ffs/PAGE_SIZE;
}

uint32 free_pt_pages(){
    struct memblk *memptr;
    uint32 free_pta;
    free_pta = 0;
    for(memptr = ptlist.mnext; memptr != NULL; memptr = memptr->mnext){
        free_pta += memptr->mlength;
    }
    kprintf("%10d bytes of PTA\n", free_pta);
    for (memptr = ptlist.mnext; memptr != NULL; memptr = memptr->mnext) {
      kprintf("[0x%08X to 0x%08X]\n", (uint32)memptr,
              ((uint32)memptr) + memptr->mlength - 1);
    }
    return 0;

}

uint32 used_ffs_frames(pid32 pid){
    return process_ffs_used[pid];
}

uint32 allocated_virtual_pages(pid32 pid) {
  uint32 i, virtualPageCount = 0;
  unsigned long pdbr = proctab[pid].pdbr;
  pd_t *pde = (pd_t *)pdbr;
  for (i = V_HEAP_PD_START; i < PTE_PER_PAGE; i++) {
    if (pde[i].pd_avail == 0)
      virtualPageCount++;
    else
      break;
  }
  return virtualPageCount+XINU_PAGES;
}

void clear_page(char *address) {
  uint32 i;
  pd_t *page_pointer = (pd_t *)address;
  for (i = 0; i < PTE_PER_PAGE; i++) {
    page_pointer[i].pd_pres = 0;   /* page table present?		*/
    page_pointer[i].pd_write = 0;  /* page is writable?		*/
    page_pointer[i].pd_user = 0;   /* is use level protection?	*/
    page_pointer[i].pd_pwt = 0;    /* write through cachine for pt?*/
    page_pointer[i].pd_pcd = 0;    /* cache disable for this pt?	*/
    page_pointer[i].pd_acc = 0;    /* page table was accessed?	*/
    page_pointer[i].pd_mbz = 0;    /* must be zero			*/
    page_pointer[i].pd_fmb = 0;    /* four MB pages?		*/
    page_pointer[i].pd_global = 0; /* global (ignored)		*/
    page_pointer[i].pd_valid = 0;  /* for programmer's use		10*/
    page_pointer[i].pd_alloc = 0;  /* for programmer's use		11*/
    page_pointer[i].pd_avail = 1;  /* for programmer's use		12*/
    page_pointer[i].pd_base = 0;   /* location of page table?	*/
  }
  return;
}

void print_page_dir(char* pageAddress, pid32 pid){
  pd_t *printPage = (pd_t*)pageAddress;
  uint32 stopPoint = 0;
  uint32 i;
  for(i = 0; i < PTE_PER_PAGE; i++){
    if(printPage[i].pd_pres == 1){
     kprintf("Base address of directory entry: %08X\n", printPage[i].pd_base);
      stopPoint = i;
    }
  }
  kprintf("\n****DIRECTORY STATS****\n");
  kprintf("Directory for process %d\n", pid);
  kprintf("Address in PTA: %08X\n", pageAddress);
  kprintf("Number of page entries: %d\n", stopPoint);
  kprintf("***********************\n");
  return;
}

void print_page_table(char* pageAddress, pid32 pid){
  pd_t *printPage = (pd_t*)pageAddress;
  uint32 stopPoint = 0;
  uint32 i;
  for(i = 0; i < PTE_PER_PAGE; i++){
    if(printPage[i].pd_pres == 1){
      stopPoint = i;
    }
  }
  kprintf("\n****DIRECTORY STATS****\n");
  kprintf("Directory for process %d\n", pid);
  kprintf("Address in PTA: %08X\n", pageAddress);
  kprintf("Number of page entries: %d\n", stopPoint);
  kprintf("***********************\n");
  return;
}
