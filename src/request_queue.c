#ifndef QUEUE_REQ_C
#define QUEUE_REQ_C

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "os-challenge-util.h"
#include "request_node.h"
#include "request_queue.h"

void enqueue_request(queue_req_t *q, request_t *req){
    node_req_t *new_node = malloc(sizeof(node_req_t));
    new_node->req = req;
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

request_t *dequeue_request(queue_req_t *q){
    // If priority queue is empty
    if(q->head == NULL){
        printf("The queue is empty!");
        return NULL;
    }

    // Move head-pointer to next node
    request_t *result = q->head->req;
    node_req_t *temp = q->head;
    q->head = q->head->next;
    
    // If queue is empty after dequeue, set tail to NULL
    if(q->head == NULL){
        q->tail = NULL;
    }

    free(temp);
    return result;
}

#endif // QUEUE_REQ_C