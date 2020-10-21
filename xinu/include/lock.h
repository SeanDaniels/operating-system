/* lock.h */
#define NPSINLOCKS 20
#define NLOCKS 20
#define LOCKED 1 
#define UNLOCKED 0
#define NOOWNER 0

typedef struct{
    uint32 lock_value;
    pid32 lock_owner;
} sl_lock_t;

typedef struct{
    uint32 flag;
    uint32 guard;
    qid16 queue;
}lock_t;


