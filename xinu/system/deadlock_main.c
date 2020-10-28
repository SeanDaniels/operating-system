#include "../include/xinu.h"
#include <stdio.h>
//#define DBG
#define DEADLOCK
//#define TRYLOCK

process lock_routine_a(al_lock_t *l_a, al_lock_t *l_b) {
  uint32 i;
  for (i = 0; i < 2; i++) {
    al_lock(l_a);
#ifdef DBG
    kprintf("Process %d has aquired lock %X\n", currpid, l_a->lock_number);
#endif
    run_for_ms(1000);
    al_lock(l_b);
#ifdef DBG
    kprintf("Process %d has aquired lock %X\n", currpid, l_b->lock_number);
#endif
    run_for_ms(1000);
    al_unlock(l_b);
    al_unlock(l_a);
  }
  return OK;
}

process lock_routine_b(al_lock_t *l_a, al_lock_t *l_b) {
  uint32 i;
  for (i = 0; i < 2; i++) {
    al_lock(l_b);
#ifdef DBG
    kprintf("Process %d has aquired lock %X\n", currpid, l_b->lock_number);
#endif
    run_for_ms(1000);
    al_lock(l_a);
#ifdef DBG
    kprintf("Process %d has aquired lock %X\n", currpid, l_a->lock_number);
#endif
    run_for_ms(1000);
    al_unlock(l_a);
    al_unlock(l_b);
  }
  return OK;
}

process lock_routine_c(al_lock_t *l_a, al_lock_t *l_b) {
  uint32 i;
  bool8 temp;
  for (i = 0; i < 2; i++) {
    temp = al_trylock(l_a);
#ifdef DBG
    kprintf("Process %d has aquired lock %X\n", currpid, l_a->lock_number);
#endif
    run_for_ms(1000);
    al_trylock(l_b);
#ifdef DBG
    kprintf("Process %d has aquired lock %X\n", currpid, l_b->lock_number);
#endif
    run_for_ms(1000);
    al_unlock(l_b);
    al_unlock(l_a);
  }
  return OK;
}

process lock_routine_d(al_lock_t *l_a, al_lock_t *l_b) {
  uint32 i;
  bool8 temp;
  for (i = 0; i < 2; i++) {
    al_trylock(l_b);
#ifdef DBG
    kprintf("Process %d has aquired lock %X\n", currpid, l_b->lock_number);
#endif
    run_for_ms(1000);
    al_trylock(l_a);
#ifdef DBG
    kprintf("Process %d has aquired lock %X\n", currpid, l_a->lock_number);
#endif
    run_for_ms(1000);
    al_unlock(l_a);
    al_unlock(l_b);
  }
  return OK;
}

/* process main(void) { */
/*   pid32 pid1, pid2; */
/*   al_lock_t l_a, l_b; */
/*   al_initlock(&l_a); */
/*   al_initlock(&l_b); */
/* #ifdef DEADLOCK */
/*   pid32 pid3, pid4; */
/*   al_lock_t l_c, l_d; */
/*   al_initlock(&l_c); */
/*   al_initlock(&l_d); */
/*   pid1 = create((void *)lock_routine_a, INITSTK, 2, "lr_a", 2, &l_a, &l_b);
 */
/*   pid2 = create((void *)lock_routine_b, INITSTK, 2, "lr_b", 2, &l_a, &l_b);
 */
/*   pid3 = create((void *)lock_routine_a, INITSTK, 1, "lr_a", 2, &l_c, &l_d);
 */
/*   pid4 = create((void *)lock_routine_b, INITSTK, 1, "lr_a", 2, &l_c, &l_d);
 */
/*   resume(pid1); */
/*   sleepms(500); */
/*   resume(pid2); */
/*   sleepms(500); */
/*   resume(pid3); */
/*   sleepms(500); */
/*   resume(pid4); */
/*   receive(); */
/*   receive(); */
/*   receive(); */
/*   receive(); */
/* #endif */
/* #ifdef TRYLOCK */
/*   pid1 = create((void *)lock_routine_c, INITSTK, 2, "lr_c", 2, &l_a, &l_b);
 */
/*   pid2 = create((void *)lock_routine_d, INITSTK, 2, "lr_d", 2, &l_a, &l_b);
 */
/*   resume(pid1); */
/*   sleepms(500); */
/*   resume(pid2); */
/*   receive(); */
/*   receive(); */
/* #endif */
/*   sync_printf("\nDone\n"); */
/*   return OK; */
/* } */
