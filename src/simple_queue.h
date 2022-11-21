#pragma once 

#include "os-challenge-util.h"
#include <stdbool.h>



typedef struct node {
    int *conn_sd;
    struct node *next;
} node_t;

typedef struct task {
    int id; 
    int sd;
    uint8_t hash[SHA256_DIGEST_LENGTH];
    uint64_t start; 
    uint64_t end;
    uint8_t prio;
} task_t;

typedef struct task_node {
    task_t *task;
    struct task_node *next;
} task_node_t; 

typedef struct request_node {
    request_t *req;
    struct request_node *next;
} request_node_t; 

typedef struct mod_response_node {
    mod_response_t *res;
    struct mod_response_node *next;
} mod_response_node_t; 


void enqueue(int *conn_sd);
int *dequeue();

void enqueue_task(task_t *ptask);
task_t *dequeue_task();

void enqueue_req(request_t *preq);
request_t *dequeue_req();

void enqueue_res(mod_response_t *pres);
mod_response_t *dequeue_res();