#include "../include/xinu.h"

syscall initlock(lock_t *l){
    l->flag = 0;
    l->guard = 0;
    l->q = newqueue();
    return OK;
}
syscall lock(lock_t *l){
    while(test_and_set(&(l->guard), 1) == 1);
    if(l->flag==0){
        l->flag = 1;
        l->guard = 0;
    }
    else{
        insert(currpid, l->q, (int32)currpid);
        l->guard = 0;
        sleep(currpid);
    }
    return OK;
}
syscall unlock(lock_t *l){
    while(test_and_set(&(l->guard), 1) == 1);
    if(isempty(l->q)){
        l->flag = 0;
    }
    else{
        unsleep(getfirst(l->q));
    }
    l->guard = 0;
    return OK;
}

