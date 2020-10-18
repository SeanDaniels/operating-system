#include "../include/xinu.h"

void assembly_test(uint32 someValA, uint32 someValB){
    uint32 *ebpValue;
    uint32 *valueHolderA, *valueHolderB;
    asm("movl %%ebp, %0": "=r"(ebpValue));
    asm("movl 8(%%ebp), %0": "=r"(valueHolderA));
    asm("movl 12(%%ebp), %0": "=r"(valueHolderB));
    kprintf("%X\n%d\n%d\n", ebpValue, valueHolderA, valueHolderB);
    return;
}
