#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "os-challenge-util.h"
#include "simple_queue.h"

// Queue to control work flow of tasks
typedef struct task_queue{
    task_node_t *head;
    task_node_t *tail;
    struct task_queue* next;
    struct task_queue* prev;
    uint8_t priority;
}task_queue_t;

void enqueue_task_q(task_queue_t *q, task_t *t);
task_t *dequeue_task_q(task_queue_t *q);

#endif // TASK_QUEUE_H