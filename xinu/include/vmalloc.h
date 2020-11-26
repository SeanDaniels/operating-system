#ifndef __VMALLOC_H_
#define __VMALLOC_H_

char* vmalloc(uint32 nbytes);

uint32 check_page_directory(pid32 pid);

void update_page_directory(unsigned long processPDBR, unsigned long pdBaseStart, uint32 index, uint32 pages);

void update_page_table(unsigned long pageTableStartAddress, uint32 pageTableEntries);

syscall vfree(char* pointer, uint32 nbytes);
#endif // __VMALLOC_H_
