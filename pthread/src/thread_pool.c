#include "../include/thread_pool.h"

static void *worker_loop(void *arg)
{
    thread_pool_t *tp = (thread_pool_t *)arg;
    for (;;)
    {
        pthread_mutex_lock(&tp->mtx);
        while (!tp->shutting_down && tp->head == NULL)
        {
            pthread_cond_wait(&tp->cv, &tp->mtx);
        }
        if (tp->shutting_down && tp->head == NULL)
        {
            pthread_mutex_unlock(&tp->mtx);
            return NULL;
        }
        work_item_t *it = tp->head;
        tp->head = it->next;
        if (!tp->head)
            tp->tail = NULL;
        tp->active++;
        pthread_mutex_unlock(&tp->mtx);

        it->fn(it->arg);
        free(it);

        pthread_mutex_lock(&tp->mtx);
        tp->active--;
        if (!tp->active && !tp->head)
        {
            pthread_cond_broadcast(&tp->cv);
        }
        pthread_mutex_unlock(&tp->mtx);
    }
}

int tp_init(thread_pool_t *tp, int threads)
{
    memset(tp, 0, sizeof(*tp));
    if (pthread_mutex_init(&tp->mtx, NULL))
        return -1;
    if (pthread_cond_init(&tp->cv, NULL))
        return -1;
    tp->threads = threads;
    tp->tids = calloc(threads, sizeof(pthread_t));
    if (!tp->tids)
        return -1;
    for (int i = 0; i < threads; i++)
    {
        if (pthread_create(&tp->tids[i], NULL, worker_loop, tp))
            return -1;
    }
    return 0;
}

void tp_submit(thread_pool_t *tp, work_fn fn, void *arg)
{
    work_item_t *it = (work_item_t *)calloc(1, sizeof(work_item_t));
    if (!it)
        DIE("sin memoria para work_item");
    it->fn = fn;
    it->arg = arg;

    pthread_mutex_lock(&tp->mtx);
    if (tp->tail)
        tp->tail->next = it;
    else
        tp->head = it;
    tp->tail = it;
    pthread_cond_signal(&tp->cv);
    pthread_mutex_unlock(&tp->mtx);
}

void tp_wait(thread_pool_t *tp)
{
    pthread_mutex_lock(&tp->mtx);
    while (tp->head || tp->active)
    {
        pthread_cond_wait(&tp->cv, &tp->mtx);
    }
    pthread_mutex_unlock(&tp->mtx);
}

void tp_destroy(thread_pool_t *tp)
{
    pthread_mutex_lock(&tp->mtx);
    tp->shutting_down = true;
    pthread_cond_broadcast(&tp->cv);
    pthread_mutex_unlock(&tp->mtx);

    for (int i = 0; i < tp->threads; i++)
    {
        pthread_join(tp->tids[i], NULL);
    }
    free(tp->tids);
    pthread_mutex_destroy(&tp->mtx);
    pthread_cond_destroy(&tp->cv);
}
