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



task_node_t* head_task = NULL; 
task_node_t* butt_task = NULL; 

void enqueue_task(task_t *ptask) 
{
    task_node_t *newnode = malloc(sizeof(task_node_t));
    newnode->task = ptask;
    newnode->next = NULL;
    if (butt_task == NULL)
        head_task = newnode;
    else 
        butt_task->next = newnode;
    
    butt_task = newnode;
}

task_t *dequeue_task() 
{
    if (head_task == NULL)
        return NULL;

    task_t *result = head_task->task;
    task_node_t *temp = head_task; 
    head_task = head_task->next;
    if (head_task == NULL)
        butt_task = NULL;
    free(temp);
    return result; 
}