#include <pthread.h>
#include <stdlib.h>
#include "thread_pool.h"


static tpool_work_t *tpool_work_create(thread_func_t func, void *arg)
{
    tpool_work_t *work;

    if (func == NULL)
        return NULL;

    work       = malloc(sizeof(*work));
    work->func = func;
    work->arg  = arg;
    work->next = NULL;
    return work;
}

bool tpool_add_work(tpool_t *tp, thread_func_t func, void *arg)
{
    tpool_work_t *work;

    if (tp == NULL)
        return false;

    work = tpool_work_create(func, arg);
    if (work == NULL)
        return false;

    pthread_mutex_lock(&(tp->work_mutex));
    if (tp->work_first == NULL) {
        tp->work_first = work;
        tp->work_last  = tp->work_first;
    } else {
        tp->work_last->next = work;
        tp->work_last       = work;
    }

    pthread_cond_broadcast(&(tp->work_cond));
    pthread_mutex_unlock(&(tp->work_mutex));

    return true;
}

static void tpool_work_destroy(tpool_work_t *work)
{
    if (work == NULL)
        return;
    free(work);
}

static tpool_work_t *tpool_work_get(tpool_t *tp)
{
    tpool_work_t *work;

    if (tp == NULL)
        return NULL;

    work = tp->work_first;
    if (work == NULL)
        return NULL;

    if (work->next == NULL) {
        tp->work_first = NULL;
        tp->work_last  = NULL;
    } else {
        tp->work_first = work->next;
    }

    return work;
}

static void *tpool_worker(void *arg)
{
    tpool_t      *tp = arg;
    tpool_work_t *work;

    for(;;) {
        pthread_mutex_lock(&(tp->work_mutex));
            while (tp->work_first == NULL && !tp->terminate)
                pthread_cond_wait(&(tp->work_cond), &(tp->work_mutex));

            if (tp->terminate)
                break;

            work = tpool_work_get(tp);
            tp->working_cnt++;
        pthread_mutex_unlock(&(tp->work_mutex));

        if (work != NULL) {
            work->func(work->arg);
            tpool_work_destroy(work);
        }

        pthread_mutex_lock(&(tp->work_mutex));
            tp->working_cnt--;
            if (!tp->terminate && tp->working_cnt == 0 && tp->work_first == NULL)
                pthread_cond_signal(&(tp->working_cond));
        pthread_mutex_unlock(&(tp->work_mutex));
    }

    tp->thread_cnt--;
    pthread_cond_signal(&(tp->working_cond));
    pthread_mutex_unlock(&(tp->work_mutex));
    return NULL;
}



tpool_t *tpool_create(size_t nthreads)
{
    tpool_t   *tm;
    pthread_t  thread;

    if (nthreads == 0)
        nthreads = 2;

    tm             = calloc(1, sizeof(*tm));
    tm->thread_cnt = nthreads;

    pthread_mutex_init(&(tm->work_mutex), NULL);
    pthread_cond_init(&(tm->work_cond), NULL);
    pthread_cond_init(&(tm->working_cond), NULL);

    tm->work_first = NULL;
    tm->work_last  = NULL;

    for (size_t i = 0; i < nthreads; ++i) {
        pthread_create(&thread, NULL, tpool_worker, tm);
        pthread_detach(thread);
    }

    return tm;
}

void tpool_wait(tpool_t *tp)
{
    if (tp == NULL)
        return;

    pthread_mutex_lock(&(tp->work_mutex));
    for(;;) {
        if ((!tp->terminate && tp->working_cnt != 0) || (tp->terminate && tp->thread_cnt != 0)) {
            pthread_cond_wait(&(tp->working_cond), &(tp->work_mutex));
        } else {
            break;
        }
    }
    pthread_mutex_unlock(&(tp->work_mutex));
}

void tpool_destroy(tpool_t *tp)
{
    tpool_work_t *work;
    tpool_work_t *work2;

    if (tp == NULL)
        return;

    pthread_mutex_lock(&(tp->work_mutex));
    work = tp->work_first;
    while (work != NULL) {
        work2 = work->next;
        tpool_work_destroy(work);
        work = work2;
    }
    tp->terminate = true;
    pthread_cond_broadcast(&(tp->work_cond));
    pthread_mutex_unlock(&(tp->work_mutex));

    tpool_wait(tp);

    pthread_mutex_destroy(&(tp->work_mutex));
    pthread_cond_destroy(&(tp->work_cond));
    pthread_cond_destroy(&(tp->working_cond));

    free(tp);
}
