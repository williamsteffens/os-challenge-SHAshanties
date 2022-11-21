#ifndef TASK_PRIORITY_QUEUE_C
#define TASK_PRIORITY_QUEUE_C

#define DEBUG 0

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "os-challenge-util.h"
#include "task_queue.h"
#include "task_priority_queue.h"

// Creates a *task_queue_t to a new queue in the PQ 
void enqueue_queue(task_priority_queue_t *pq){
    task_queue_t *new_queue = malloc(sizeof(task_queue_t));
    
    new_queue->next = NULL;
    new_queue->prev = NULL;
    
    // First queue has priority 1
    if(pq->tail == NULL){
        pq->head = new_queue;
        new_queue->priority = 1;
    }
    // Following appended queues get priority of tail + 1
    else{
        pq->tail->next = new_queue;
        new_queue->prev = pq->tail;
        new_queue->priority = pq->tail->priority + 1; 
    }

    // New queue becomes tail of priority queue
    pq->tail = new_queue;
}

// Dequeues head *task_queue_t from the PQ 
void dequeue_queue(task_priority_queue_t *pq){
    // If priority queue is empty
    if(pq->head == NULL){
        printf("The priority queue is empty!\n");
        return;
    }

    task_queue_t *result_queue = pq->head;
    task_t *result_task = dequeue_task_q(result_queue);
    
    // free() the queue's nodes, discarding the tasks
    while(result_task != NULL){
        result_task = dequeue_task_q(result_queue);
    }

    // Move head-pointer to next queue
    pq->head = pq->head->next;
    
    // If priority queue is empty after dequeue, set tail to NULL
    if(pq->head == NULL){
        pq->tail = NULL;
    }

    free(result_queue);
}

// Initialises a PQ with n *task_queue_t
void init_priority_queue(task_priority_queue_t *pq, int n){
    // Initialise pq with n queues
    for(int i = 0; i < n; i++){
        enqueue_queue(pq);
    }
}

// Clears all *task_nodes in each *task_queue_t and frees everything, including the PQ itself
void free_priority_queue(task_priority_queue_t *pq){
    while(pq->head != NULL){
        dequeue_queue(pq);
    }
    dequeue_queue(pq);
    free(pq);
}

// Enqueues a *task_t into a *task_queue_t with same priority in the PQ
void enqueue_task_pq(task_priority_queue_t *pq, task_t *t){
    //TODO: Check if the queue can be found in O(1), by adding 48 * n to the address
    // The queues' addresses are shifted by 48, starting from head-pointer
    //task_queue_t *q = pq->head + (48 * t->prio) - 48;
    // Maybe look at this link?: https://www.tutorialspoint.com/cprogramming/c_pointer_arithmetic.htm
    
    task_queue_t *q = pq->head;
    
    // Traverse PQ until q with same priority is found    
    while(q->priority != t->prio){
        q = q->next;
    }
    enqueue_task_q(q, t);
}

// Dequeues a *task_t from the highest priority non-empty *task_queue_t in the PQ and returns it
task_t *dequeue_task_pq(task_priority_queue_t *pq){
    task_queue_t *q = malloc(sizeof(task_queue_t)); 
    q = pq->tail;
    // Find highest priority non-empty queue
    while(q->head == NULL){
        q = q->prev;
        if(q->priority == 1){
            break;
        }
    }
    return dequeue_task_q(q);
}

/* TESTS TO CHECK FUNCTIONALITY */
#if DEBUG 
    // Tests: enqueue_queue() and dequeue_queue() 
    void print_test_1(task_priority_queue_t *pq){
        printf("  PQ:                %p\n", (void *)pq);
        printf("  PQ head:           %p\n", (void *)pq->head);
        printf("  PQ tail:           %p\n", (void *)pq->tail);
        //printf("  PQ head prio: %u\n", pq->head->priority);

        printf("\nENQUEUE (pq)\n");
        enqueue_queue(pq);

        printf("  PQ:                %p\n", (void *)pq);
        printf("  PQ head:           %p\n", (void *)pq->head);
        printf("  PQ head prio:      %u\n", pq->head->priority);
        printf("  PQ tail:           %p\n", (void *)pq->tail);
        printf("  PQ tail prio:      %u\n", pq->tail->priority);

        printf("\nENQUEUE (pq)\n");
        enqueue_queue(pq);

        printf("  PQ:                %p\n", (void *)pq);
        printf("  PQ head:           %p\n", (void *)pq->head);
        printf("  PQ head prio:      %u\n", pq->head->priority);
        printf("  PQ tail:           %p\n", (void *)pq->tail);
        printf("  PQ tail prio:      %u\n", pq->tail->priority);
        printf("  PQ tail prev:      %p\n", (void *)pq->tail->prev);
        printf("  PQ tail prev prio: %u\n", pq->tail->prev->priority);

        printf("\nDEQUEUE (pq)\n");
        dequeue_queue(pq);

        printf("  PQ:                %p\n", (void *)pq);
        printf("  PQ head:           %p\n", (void *)pq->head);
        printf("  PQ head prio:      %u\n", pq->head->priority);
        printf("  PQ tail:           %p\n", (void *)pq->tail);
        printf("  PQ tail prio:      %u\n", pq->tail->priority);

        printf("\nDEQUEUE (pq)\n");
        dequeue_queue(pq);

        printf("\nDEQUEUE (pq)\n");
        dequeue_queue(pq);
    }

    // Tests: init_priority_queue()
    void print_test_2(task_priority_queue_t *pq){
        int n = 16;
        init_priority_queue(pq, n);
        printf("%d queues are initialised...\n\n", n);
        printf("  PQ:           %p\n", (void *)pq);
        printf("  PQ head:      %p\n", (void *)pq->head);
        printf("  PQ head prio: %u\n", pq->head->priority);
        printf("  PQ tail:      %p\n", (void *)pq->tail);
        printf("  PQ tail prio: %u\n", pq->tail->priority);
    }

    // Tests: free_priority_queue()
    void print_test_3(task_priority_queue_t *pq){
        print_test_2(pq);
        free_priority_queue(pq);
        printf("%p\n", pq);
    }

    // Tests: enqueue_task_pq()
    void print_test_4(task_priority_queue_t *pq){
        print_test_2(pq);

        task_t *t= malloc(sizeof(task_t));
        t->prio = 5;
        
        // Pointer for easy access to the enqueued queue
        task_queue_t *q = malloc(sizeof(task_queue_t));
        q = pq->head;
        while(q->priority != t->prio){
            q = q->next;
        }

        printf("\nENQUEUE (pq) with t\n\n");
        enqueue_task_pq(pq, t);
        printf("  PQ:            %p\n", (void *)pq);
        printf("  PQ head:       %p\n", (void *)pq->head);
        printf("  PQ head prio:  %u\n", pq->head->priority);
        printf("  PQ Q:          %p\n", (void *)q);
        printf("  PQ Q prio:     %u\n", q->priority);
        printf("  PQ Q t:        %p\n", (void *)q->head->task);
        printf("  PQ Q t prio:   %u\n", q->head->task->prio);
        printf("  PQ tail:       %p\n", (void *)pq->tail);
        printf("  PQ tail prio:  %u\n", pq->tail->priority);
    }

    // Tests: dequeue_task_pq()
    void print_test_5(task_priority_queue_t *pq){
        init_priority_queue(pq, 4);

        task_t *result;
        task_t *t1 = malloc(sizeof(task_t));
        task_t *t3 = malloc(sizeof(task_t));
        t1->prio = 1;
        t3->prio = 3;

        printf("\nENQUEUE (pq) with t1\n");
        enqueue_task_pq(pq, t1);
        printf("\nENQUEUE (pq) with t3\n");
        enqueue_task_pq(pq, t3);

        printf("  PQ:            %p\n", (void *)pq);
        printf("  PQ head:       %p\n", (void *)pq->head);
        printf("  PQ head prio:  %u\n", pq->head->priority);
        printf("  PQ tail:       %p\n", (void *)pq->tail);
        printf("  PQ tail prio:  %u\n", pq->tail->priority);

        printf("\nDEQUEUE (pq)\n\n");
        result = dequeue_task_pq(pq);
        printf("Result:          %p\n", (void *)result);
        printf("Result prio:     %u\n", result->prio);
    }

    int main(void){
        task_priority_queue_t *pq = malloc(sizeof(task_priority_queue_t));
        print_test_1(pq);
        print_test_2(pq);
        print_test_3(pq);
        print_test_4(pq);
        print_test_5(pq);

       return 0;
    }
#endif

#endif // TASK_PRIORITY_QUEUE_C