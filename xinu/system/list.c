#include "../include/xinu.h"
//#define DBG
struct list *new_list;

syscall listinit(void){
    new_list = (struct list *)getmem(sizeof(struct list));
    if (new_list == (struct list *)SYSERR){
            panic("newlist - insufficient memory");
        }
    new_list->head = NULL;
    new_list->tail = NULL;
    #ifdef DBG
    kprintf("\nList created\n");
    #endif
    return OK;
}

syscall test_list(void){
    struct node* found_node;
    kprintf("Adding 5 nodes to list\n");
    for(uint32 i = 0; i < 5; i++){
        append_list_node(i);
    }
    kprintf("Printing list\n");
    print_list();
    kprintf("Searching for pid = 3 in list\n");
    found_node = find_pid_in_list(3);
    kprintf("\nFound node\nAddress: %x, Content: %d, Tickets: %d, Next node: %X\n", found_node, found_node->PID, found_node->tickets, found_node->next);
    kprintf("Attempting to remove head from list\n");
    remove_from_list(0);
    kprintf("Printing new list\n");
    kprintf("Attempting to remove tail from list\n");
    remove_from_list(4);
    kprintf("Printing new list\n");
    print_list();
    kprintf("Attempting to node from middle of list\n");
    remove_from_list(2);
    kprintf("Printing new list\n");
    print_list();
    return OK;
}

uint32 append_list_node(pid32 pid){
    struct node *curr = new_list->head, *tail = new_list->tail;

    struct node* newNode = (struct node *)getmem(sizeof(struct node));

    if(newNode == (struct node*)SYSERR){
        panic("newnode - insufficient memory");
    }

    newNode->PID = pid;
    newNode->tickets = -1;
    newNode->next = NULL;
    //empty list
    if(curr==NULL){
#ifdef DBG
        kprintf("Adding node to an empty list\n");
#endif
        new_list->head = newNode;
        new_list->tail = newNode;
        newNode->next = NULL;
        return 1;
    }
    //non empty list
    else{
#ifdef DBG
        kprintf("Adding node to a non-empty list\n");
#endif
        tail->next = newNode;
        new_list->tail = newNode;
        return 1;
    }
    return 0;
}

/* return node that precedes the node being searched for */
struct node* find_pid_in_list(pid32 pid){
    struct node* curr = new_list->head;
    if(curr==NULL){
        curr = (struct node*)SYSERR;
        return curr;
    }
    while(curr!=NULL){
        if(curr->PID==pid){
            return curr;
        }
        else{
            curr = curr->next;
        }
    }
    curr = (struct node*)SYSERR;
    return curr;
}

void print_list(void){
    struct node *curr = new_list->head, *tail = new_list->tail;
    if(curr==NULL){
        kprintf("Empty list\n");
        return;
    }
    kprintf("Memloc of list head: %x\n", curr);
    kprintf("Memloc of list tail: %X\n", tail);
    while(curr!=NULL){
        kprintf("Adress of node: %X\nContent of node\nPID: %d, Tickets: %d, Eligible: %d, Next node: %X\n", curr, curr->PID, curr->tickets, curr->elig, curr->next);
        curr = curr->next;
    }
    return;
}

void remove_from_list(pid32 pid){
    struct node* curr = new_list->head;
    struct node* prev = curr;
    if(curr==NULL){
#ifdef DBG
        kprintf("Empty list, returning\n");
#endif
        return;
    }
    while(curr->PID!=pid){
        if(curr->next==NULL){
#ifdef DBG
            kprintf("Value not found, returning\n");
#endif
            return;
        }
        else{
            prev = curr;
            curr = curr->next;
        }
    }
    if(curr==new_list->head){
        new_list->head = curr->next;
    }
    if(curr==new_list->tail){
        new_list->tail = prev;
        prev->next = NULL;
    }
    else{
        prev->next = curr->next;
    }
    return;

}

uint32 insert_list_node(pid32 pid, uint32 number_of_tickets){
    struct node* curr = new_list->head;
    struct node* prev = curr;
    uint32 insertBefore = 0;
    uint32 insertAfter = 0;
    //create new node
    struct node* new_node = (struct node *)getmem(sizeof(struct node));
    //check new node creation
    if (new_node==(struct node*)SYSERR){
        panic("Error: cant add node\n");
        return -1;
    }
    //set new node values
    new_node->PID=pid;
    new_node->tickets=number_of_tickets;
    new_node->elig = NOT_ELIGIBLE;
    new_node->next=NULL;

    // empty list
    if(curr==NULL){
        new_list->head = new_node;
        new_list->tail = new_node;
        return 1;
    }

    //non-empty list, new_node has the greatest number of tickets
    if(new_node->tickets>curr->tickets){
        new_list->head = new_node;
        new_node->next = curr;
        return 1;
    }

    //find appropriate location in list
    while(curr!=NULL){
        //existing node's tickets are less than or equal to new nodes tickets
        if(curr->tickets<number_of_tickets){
            insertBefore = 1;
            break;
        }
        prev = curr;
        curr = curr->next;
       
    }
    //if insert before = 1, a node with less tickets was found
    if(insertBefore){
        prev->next = new_node;
        new_node->next = curr;
        return 1;
    }
    if(curr==NULL){
        prev->next = new_node;
        new_list->tail = new_node;
        return 1;
    }
    return -1;
}
