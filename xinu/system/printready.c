#include "../include/xinu.h"
//#define DBG
syscall print_ready_list(){
    qid16 head = queuehead(readylist);
    qid16 tail = queuetail(readylist);
    qid16 head_next = queuetab[head].qnext;
    qid16 next = head_next;
    qid16 tail_next = queuetab[tail].qnext;
    #ifdef DBG
    kprintf("Head of ready list: %d\n", head);
    kprintf("Tail of ready list: %d\n", tail);
    kprintf("Head of ready list points to: %d\n", head_next);
    kprintf("Tail of ready list points to: %d\n", tail_next);
    #endif
    kprintf("Ready list entries:\n");
    while (next!=tail){
        kprintf("PID: %d\n", next);
        next = queuetab[next].qnext;
    }
    return 0;
}
