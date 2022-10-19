#pragma once 

#include "os-challenge-util.h"
#include <stdbool.h>

typedef struct node {
    int *conn_sd;
    struct node *next;
} node_t;

typedef struct task {
    int sd;
    uint8_t hash[SHA256_DIGEST_LENGTH];
    uint64_t start; 
    uint64_t end;
    bool *done;
} task_t;

typedef struct task_node {
    task_t *task;
    struct task_node *next;
} task_node_t; 


void enqueue(int *conn_sd);
int *dequeue();

void enqueue_task(task_t *ptask);
task_t *dequeue_task();