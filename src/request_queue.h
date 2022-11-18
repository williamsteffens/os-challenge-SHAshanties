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

void enqueue_req(queue_req_t q, request_t *req);
request_t *dequeue_req(queue_req_t q);
