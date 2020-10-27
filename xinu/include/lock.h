/* lock.h */
#define NPSINLOCKS 20
#define NLOCKS 20
#define NALOCKS 20
#define NPILOCKS 20

#define LOCKED 1
#define UNLOCKED 0
#define NOOWNER 500

typedef struct {
  uint32 lock_value;
  pid32 lock_owner;
} sl_lock_t;

typedef struct {
  uint32 flag;
  uint32 guard;
  qid16 queue;
} lock_t;

typedef struct {
  uint32 flag;
  uint32 guard;
  pid32 owner;
  uint32 lock_number;
  qid16 queue;
} al_lock_t;

typedef struct {
  uint32 flag;
  uint32 guard;
  pid32 owner;
  uint32 lock_number;
  qid16 queue;
  pri16 og_priority;
  bool8 inherited_priority;
} pi_lock_t;

syscall setPark();
syscall park();
syscall unpark(pid32 thread);

syscall al_initlock(al_lock_t *l);
syscall al_lock(al_lock_t *l);
syscall al_unlock(al_lock_t *l);
syscall al_park(al_lock_t *l, int dep);
syscall al_unpark(pid32 thread);
int32 get_owner(al_lock_t *l, pid32 rootProcess);
bool8 al_trylock(al_lock_t *l);
void print_deadlock();
extern bool8 deadlock_chain[NPROC];

syscall pi_initlock(pi_lock_t *l);
syscall pi_lock(pi_lock_t *l);
syscall pi_unlock(pi_lock_t *l);
syscall pi_park(pi_lock_t *l, bool8 prio);
syscall pi_unpark(pid32 thread);
void print_priority_shift();
bool8 check_priority(pi_lock_t *l, pid32 currpid);
void revert_priority(pi_lock_t *l);
