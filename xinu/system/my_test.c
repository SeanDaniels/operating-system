#include "../include/xinu.h"
//#define TEST1
#define TEST2
#define DBG
void sync_printf(char *fmt, ...)
{
        intmask mask = disable();
        void *arg = __builtin_apply_args();
        __builtin_apply((void*)kprintf, arg, 100);
        restore(mask);
}

void process_info(pid32 pid) {
  sync_printf("\n*******PROCESS INFO*******\n");
  sync_printf("P%d:: virtual pages = %d\n", pid, allocated_virtual_pages(pid));
  sync_printf("P%d:: FFS frames = %d\n", pid, used_ffs_frames(pid));
  print_page_dir((char *)(proctab[pid].pdbr), pid);
  sync_printf("*******END PROCESS INFO*******\n\n");

}

process empty_process(){
	process_info(currpid);
	return OK;
}

process vmalloc_process() {
  intmask mask = disable();
  uint32 i;
  char *ptr1 = vmalloc(8 * PAGE_SIZE);
  char *ptr2 = vmalloc(4 * PAGE_SIZE);
  vfree(ptr1, 8* PAGE_SIZE);
  restore(mask);
  return OK;
}

process main(void) {
  uint32 i = 0;
  sync_printf("\n\nTESTS START NOW...\n");
  sync_printf("-------------------\n\n");
  /* After initialization */
#ifdef DBG
  kprintf("Address start: 0x%08X\n", START);
  kprintf("Page Table Area start: 0x%08X\n", PT_START);
  kprintf("FFS Area start: 0x%08X\n", FFS_START);
  #endif
  sync_printf("*Free FFS page at start*\n");
  sync_printf("P%d:: Free FFS pages = %d out of %d\n\n", currpid,
              free_ffs_pages(), MAX_FFS_SIZE);

#ifdef TEST1
  sync_printf("[TEST 1] P%d:: Spawning 2 processes that do not perform any vheap allocations...\n\n", currpid);

	resume(vcreate((void *)empty_process, INITSTK, 1, "p1", 0));
	sleepms(1000);
	resume(vcreate((void *)empty_process, INITSTK, 1, "p2", 0));

	receive();
	receive();

	sync_printf("P%d:: Free FFS pages = %d out of %d\n\n", currpid, free_ffs_pages(), MAX_FFS_SIZE);

#endif

#ifdef TEST2
  /* TEST2: 1 process with small allocations */
  sync_printf("[TEST 2] P%d:: Spawning 1 process that performs small "
              "allocations...\n\n",
              currpid);
  resume(vcreate((void *)vmalloc_process, INITSTK, 1, "small", 0));

  receive();
  sleepms(100);

  sync_printf("\nP%d:: Free FFS pages = %d out of %d\n\n", currpid,
              free_ffs_pages(), MAX_FFS_SIZE);
#endif
#ifdef DBG
  kprintf("Test Done\n");
#endif
  return OK;
}
