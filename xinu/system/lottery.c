#include "../include/xinu.h"
#include <stdlib.h>
//#define DBGT
//#define DBG_LOTTERY_STATS
//#define DBG_RETURN_VAL

uint32 global_tickets = 0;

void set_tickets(pid32 pid, uint32 number_of_tickets){
    if(insert_list_node(pid, number_of_tickets)){
        #ifdef DBG
        kprintf("Successfully added user process (pid %d) to list\nPrinting list:\n", pid);
        print_list();
        #endif
        return;
    }
    else{
        kprintf("Error inserting pid in list\n");
        return;
    }
}

void determine_global_ticket_count(){
    global_tickets = 0;
    struct node* curr = new_list->head;
    while(curr!=NULL){
#ifdef DBGT
        kprintf("PID: %d, Tickets: %d, Eligible: %d\n", curr->PID, curr->tickets, curr->elig);
        #endif
        if(curr->elig==ELIGIBLE){
           global_tickets+=curr->tickets; 
        }
        curr=curr->next;
    }
    return;
}
/*  Function should only include jobs currently in the ready list
 *  This function doesn't do that, it includes all jobs, ready or not
 *  */
pid32 draw_lottery_tickets(pid32 currentProcess){
    uint32 counter = 0;
    uint32 winning_ticket;
    struct node* curr = new_list->head;
    determine_global_ticket_count();
    if(!global_tickets){
        #ifdef DBG_LOTTERY_STATS
        kprintf("No global tickets\n");
        #endif
        return currentProcess;
    }
    winning_ticket = rand() % global_tickets;
#ifdef DBG_LOTTERY_STATS
    kprintf("\nGlobal number of lottery tickets: %d\nWinning lottery number: %d\n", global_tickets, winning_ticket);
#endif
    while(curr!=NULL){
        if(curr->elig==ELIGIBLE){
            counter += curr->tickets;
#ifdef DBG_TICKET_ACCUM
            kprintf("User process (pid %d) -> %d tickets\n", curr->PID, curr->tickets);
#endif
            if(counter > winning_ticket){
#ifdef DBG_RETURN_VAL
                kprintf("Winner: %d\n", curr->PID);
#endif
                return curr->PID;
            }
        }
        curr = curr->next;
    }
    return currentProcess;
}
