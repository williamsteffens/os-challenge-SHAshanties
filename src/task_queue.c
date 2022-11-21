#ifndef QUEUE_REQ_C
#define QUEUE_REQ_C

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "os-challenge-util.h"
//#include "task_node.h"
#include "task_queue.h"

void enqueue_task_q(task_queue_t *q, task_t *t){
    task_node_t *new_node = malloc(sizeof(task_node_t));
    new_node->task = t;
    new_node->next = NULL;

    // First node in queue
    if(q->tail == NULL){
        q->head = new_node;
    }
    // Following appended nodes
    else{
        q->tail->next = new_node;
    }
    
    // New node becomes tail in queue
    q->tail = new_node;    
}

task_t *dequeue_task_q(task_queue_t *q){
    // If priority queue is empty
    if(q->head == NULL){
        //printf("The queue with priority %u is empty!\n", q->priority);
        return NULL;
    }

    // Move head-pointer to next node
    task_t *result = q->head->task;
    task_node_t *temp = q->head;
    q->head = q->head->next;
    
    // If queue is empty after dequeue, set tail to NULL
    if(q->head == NULL){
        q->tail = NULL;
    }

    free(temp);
    return result;
}

#endif // QUEUE_REQ_C