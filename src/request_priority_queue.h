#ifndef PRIORITY_QUEUE_REQ_H
#define PRIORITY_QUEUE_REQ_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "os-challenge-util.h"
#include "request_node.h"
#include "request_queue.h"

typedef struct priority_queue_req{
    //queue_req_t *queues;
    queue_req_t **queues;
}priority_queue_req_t;

/*
priority_queue_req_t init_pq();
void enqueue_pq(priority_queue_req_t pq, request_t *req);    
request_t *dequeue_pq(priority_queue_req_t pq, int prio);
*/
#endif // PRIORITY_QUEUE_REQ_H