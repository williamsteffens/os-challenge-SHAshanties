#ifndef TASK_PRIORITY_QUEUE_H
#define TASK_PRIORITY_QUEUE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "os-challenge-util.h"
#include "task_queue.h"

typedef struct task_priority_queue{
    task_queue_t *head;
    task_queue_t *tail;
}task_priority_queue_t;

void enqueue_queue(task_priority_queue_t *pq);
void dequeue_queue(task_priority_queue_t *pq);
void init_priority_queue(task_priority_queue_t *pq, int n);
void free_priority_queue(task_priority_queue_t *pq);
void enqueue_task_pq(task_priority_queue_t *pq, task_t *t);
task_t *dequeue_task_pq(task_priority_queue_t *pq);

#endif // TASK_PRIORITY_QUEUE_H