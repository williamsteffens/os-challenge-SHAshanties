#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "os-challenge-util.h"
#include "request_node.h"
#include "request_queue.h"

void enqueue_req(queue_req_t q, request_t *req){
    q.head;
    q.tail;
    node_req_t *new_node = malloc(sizeof(node_req_t));
    new_node->req = req;
    new_node->next = NULL;

    if (q.tail == NULL)
        q.head = new_node;
    else 
        q.tail->next = new_node;
    
    q.tail = new_node;    
}

request_t *dequeue_req(queue_req_t q){
    if(q.head == NULL){
        return NULL;
    }

    request_t *result = q.head->req;
    node_req_t *temp = q.head;
    q.head = q.head->next;
    
    if(q.head == NULL){
        q.tail = NULL;
    }

    free(temp);
    return result;
}
