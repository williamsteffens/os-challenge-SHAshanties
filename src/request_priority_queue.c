#ifndef PRIORITY_QUEUE_REQ_C
#define PRIORITY_QUEUE_REQ_C

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "os-challenge-util.h"
#include "request_node.h"
#include "request_queue.h"
#include "request_priority_queue.h"

priority_queue_req_t *init_pq(){
    // TODO: Need to check how to handle 16 queues exactly
    //priority_queue_req_t pq;
    //return pq;
    priority_queue_req_t *pq = malloc(sizeof(priority_queue_req_t) + 16 * sizeof(queue_req_t));
    return pq;
}

// The request is inserted into a pq accordingly with priority
void enqueue_pq(priority_queue_req_t *pq, request_t *req){
    printf("1?\n");
    int pos = req->prio;
    printf("pos: %d\n", pos);
    enqueue_request(*(pq->queues[pos]), req);
    printf("2?\n");
}

request_t *dequeue_pq(priority_queue_req_t pq, int prio){
    return dequeue_request(*pq.queues[prio]);
}

// Driver code
int main(void){
    request_t *req0 = malloc(sizeof(request_t));
    request_t *req1 = malloc(sizeof(request_t));
    req0->prio = 0; 
    req1->prio = 1;

    priority_queue_req_t *pq = init_pq();   

    enqueue_pq(pq, req0);
    
    //printf("%u\n", req0->prio);
    printf("%p\n", (void *)&pq);
    printf("%p\n", (void *)&pq->queues[0]);
    printf("%p\n", (void *)&pq->queues[1]);
    printf("%p\n", (void *)&pq->queues[0]->head);
    printf("%p\n", (void *)&pq->queues[0]->head->req);
    printf("%p\n", (void *)&pq->queues[0]->head->req->prio);
    printf("Hallo?\n");
    /*
    printf("%d\n", &pq.queues[0]->head->req->prio);
    printf("%d\n", pq.queues[0]->head);
    printf("%d\n", pq.queues[0]);
    printf("%d\n", &(pq.queues[0]));    
    dequeue_pq(pq, 0);
    printf("%d\n", pq.queues[0]->head);
    printf("%d\n", pq.queues[0]);
    printf("%d\n", &(pq.queues[0]));
    */    
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