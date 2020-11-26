#include "../include/xinu.h"

/* Memory bounds */

void *minheap; /* Start of heap			*/
void *maxheap; /* Highest valid heap address		*/

/*------------------------------------------------------------------------
 * meminit - initialize memory bounds and the free memory list
 *------------------------------------------------------------------------
 */
void meminit(void) {

  struct memblk *memptr; /* Ptr to memory block		*/

  /* Initialize the free memory list */

  /* Note: we pre-allocate  the "hole" between 640K and 1024K */
  // maxheap already initialized in i386.c
  //       maxheap = (void *)0x600000;	/* Assume 64 Mbytes for now */
  minheap = &end;

  memptr = memlist.mnext = (struct memblk *)roundmb(minheap);

  if ((char *)(maxheap + 1) > HOLESTART) {
    /* create two blocks that straddle the hole */
    memptr->mnext = (struct memblk *)HOLEEND;
    memptr->mlength = (int)truncmb((unsigned)HOLESTART - (unsigned)&end - 4);
    memptr = (struct memblk *)HOLEEND;
    memptr->mnext = (struct memblk *)NULL;
    memptr->mlength = (int)truncmb((uint32)maxheap - (uint32)HOLEEND - NULLSTK);
  }
  else {
    /* initialize free memory list to one block */
    memlist.mnext = memptr = (struct memblk *)roundmb(&end);
    memptr->mnext = (struct memblk *)NULL;
    memptr->mlength = (uint32)truncmb((uint32)maxheap - (uint32)&end - NULLSTK);
  }
//initialize pt area memory chunks
  memptr = ptlist.mnext = (struct memblk *)(XINU_PAGES * PAGE_SIZE);
  ptlist.mlength = MAX_PT_SIZE * PAGE_SIZE;
  memptr->mnext = (struct memblk *) NULL;
  memptr->mlength = MAX_PT_SIZE * PAGE_SIZE;

//initialize ffs area memory chunks
  memptr = ffslist.mnext =
      (struct memblk *)((XINU_PAGES + MAX_PT_SIZE) * PAGE_SIZE);
  ffslist.mlength = MAX_FFS_SIZE * PAGE_SIZE;
  memptr->mnext = (struct memblk *) NULL;
  memptr->mlength = MAX_FFS_SIZE * PAGE_SIZE;

  return;
}
