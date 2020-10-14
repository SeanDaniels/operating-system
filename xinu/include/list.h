#define ELIGIBLE 1
#define NOT_ELIGIBLE 0

struct node {
    uint32 PID; /*  pid of process in list */
    uint32 tickets; /*  number of tickets the job has */
    uint32 elig; /*  eligibility of process to participate in lottery */
    struct node *next;
};

struct list {
    struct node *head;
    struct node *tail;
};

extern struct list *new_list;
extern struct node *node;
