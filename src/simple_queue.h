#pragma once 

typedef struct node {
    struct node *next;
    int *conn_sd;
} node_t;



void enqueue(int *conn_sd);
int *dequeue();