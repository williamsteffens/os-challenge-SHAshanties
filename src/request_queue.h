#ifndef QUEUE_REQ_H
#define QUEUE_REQ_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "os-challenge-util.h"
#include "request_node.h"

// Queue to control work flow of requests
typedef struct queue_req{
    node_req_t *head;
    node_req_t *tail;
}queue_req_t;

void enqueue_request(queue_req_t *q, request_t *req);
request_t *dequeue_request(queue_req_t *q);


#endif // QUEUE_REQ_H