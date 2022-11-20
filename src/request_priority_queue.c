#ifndef PRIORITY_QUEUE_REQ_C
#define PRIORITY_QUEUE_REQ_C

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "os-challenge-util.h"
#include "request_node.h"
#include "request_queue.h"
#include "request_priority_queue.h"

void enqueue_queue(priority_queue_req_t *pq){
    queue_req_t *new_queue = malloc(sizeof(queue_req_t));
    
    new_queue->next = NULL;
    
    // First queue has priority 1
    if(pq->tail == NULL){
        pq->head = new_queue;
        new_queue->priority = 1;
    }
    // Following appended queues get priority of tail + 1
    else{
        pq->tail->next = new_queue;
        new_queue->priority = pq->tail->priority + 1; 
    }

    // New queue becomes tail of priority queue
    pq->tail = new_queue;       
}

void dequeue_queue(priority_queue_req_t *pq){
    // If priority queue is empty
    if(pq->head == NULL){
        printf("The priority queue is empty!");
        return;
    }

    // Move head-pointer to next queue
    queue_req_t *result = pq->head;
    pq->head = pq->head->next;
    
    // If priority queue is empty after dequeue, set tail to NULL
    if(pq->head == NULL){
        pq->tail = NULL;
    }

    free(result);
}

void init_priority_queue(priority_queue_req_t *pq, int n){
    // Initialise pq with n queues
    for(int i = 0; i < n; i++){
        enqueue_queue(pq);
    }
}

void enqueue_request_pq(priority_queue_req_t *pq, request_t req){
    //TODO: enqueue req into PQ's queue with same priority
}

request_t dequeue_request_pq(priority_queue_req_t *pq){
    //TODO: dequeue req from pq via priority
}

/* TESTS TO CHECK FUNCTIONALITY */
// Tests: enqueue_queue() and dequeue_queue() 
void print_test_1(priority_queue_req_t *pq){
    printf("  PQ:           %p\n", (void *)pq);
    printf("  PQ head:      %p\n", (void *)pq->head);
    printf("  PQ tail:      %p\n", (void *)pq->tail);
    //printf("  PQ head prio: %u\n", pq->head->priority);

    printf("\nENQUEUE (pq)\n\n");
    enqueue_queue(pq);

    printf("  PQ:           %p\n", (void *)pq);
    printf("  PQ head:      %p\n", (void *)pq->head);
    printf("  PQ head prio: %u\n", pq->head->priority);
    printf("  PQ tail:      %p\n", (void *)pq->tail);
    printf("  PQ tail prio: %u\n", pq->tail->priority);

    printf("\nENQUEUE (pq)\n\n");
    enqueue_queue(pq);

    printf("  PQ:           %p\n", (void *)pq);
    printf("  PQ head:      %p\n", (void *)pq->head);
    printf("  PQ head prio: %u\n", pq->head->priority);
    printf("  PQ tail:      %p\n", (void *)pq->tail);
    printf("  PQ tail prio: %u\n", pq->tail->priority);

    printf("\nDEQUEUE (pq)\n\n");
    dequeue_queue(pq);

    printf("  PQ:           %p\n", (void *)pq);
    printf("  PQ head:      %p\n", (void *)pq->head);
    printf("  PQ head prio: %u\n", pq->head->priority);
    printf("  PQ tail:      %p\n", (void *)pq->tail);
    printf("  PQ tail prio: %u\n", pq->tail->priority);
}

// Tests: init_priority_queue()
void print_test_2(priority_queue_req_t *pq){
    int n = 16;
    init_priority_queue(pq, n);
    printf("%d queues are initialised...\n\n", n);
    printf("  PQ:           %p\n", (void *)pq);
    printf("  PQ head:      %p\n", (void *)pq->head);
    printf("  PQ head prio: %u\n", pq->head->priority);
    printf("  PQ tail:      %p\n", (void *)pq->tail);
    printf("  PQ tail prio: %u\n", pq->tail->priority);
}


// Driver code
int main(void){
    priority_queue_req_t *pq = malloc(sizeof(priority_queue_req_t));
    
    //print_test_1(pq);
    //print_test_2(pq);

    return 0;
}

#endif // PRIORITY_QUEUE_REQ_C

/* Experiments:
    - Test naive approach, F.I.F.S., i.e. priority 1 stays in PQ[0] until all others are empty
    - Test timed priority, i.e. request priority increases with time, pushing onto higher PQs
    
::LINKS::
"1 queue for each priority"
* https://stackoverflow.com/questions/2921349/priority-queue-with-dynamic-priorities
"Priority scheduling"
* https://www.scaler.com/topics/operating-system/priority-scheduling-algorithm/
*/