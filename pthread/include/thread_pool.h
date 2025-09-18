#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "common.h"

typedef void (*work_fn)(void *arg);

typedef struct work_item
{
    work_fn fn;
    void *arg;
    struct work_item *next;
} work_item_t;

typedef struct
{
    pthread_mutex_t mtx;
    pthread_cond_t cv;
    work_item_t *head, *tail;
    bool shutting_down;
    int active; 
    int threads;
    pthread_t *tids;
} thread_pool_t;

int tp_init(thread_pool_t *tp, int threads);
void tp_submit(thread_pool_t *tp, work_fn fn, void *arg);
void tp_wait(thread_pool_t *tp);    
void tp_destroy(thread_pool_t *tp); 

#endif
