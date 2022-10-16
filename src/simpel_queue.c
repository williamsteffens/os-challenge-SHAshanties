#include <stdlib.h>
#include "simple_queue.h"

node_t* head = NULL; 
node_t* butt = NULL; 


void enqueue(int *conn_sd) 
{
    node_t *newnode = malloc(sizeof(node_t));
    newnode->conn_sd = conn_sd;
    newnode->next = NULL;
    if (butt == NULL)
        head = newnode;
    else 
        butt->next = newnode;
    
    butt = newnode;
}

int *dequeue() 
{
    if (head == NULL)
        return NULL;

    int *result = head->conn_sd;
    node_t *temp = head; 
    head = head->next;
    if (head == NULL)
        butt = NULL;
    free(temp);
    return result; 
}
