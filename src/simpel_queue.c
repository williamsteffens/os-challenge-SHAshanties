#include <stdlib.h>
#include "simple_queue.h"

#include <stdio.h>

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



request_node_t* head_request = NULL; 
request_node_t* butt_request = NULL; 

void enqueue_req(request_t *preq) 
{
    request_node_t *newnode = malloc(sizeof(request_node_t));
    newnode->req = preq;
    newnode->next = NULL;
    if (butt_request == NULL)
        head_request = newnode;
    else 
        butt_request->next = newnode;
    
    butt_request = newnode;
}

request_t *dequeue_req() 
{
    if (head_request == NULL)
        return NULL;

    request_t *result = head_request->req;
    request_node_t *temp = head_request; 
    head_request = head_request->next;
    if (head_request == NULL)
        butt_request = NULL;
    free(temp);
    return result; 
}



mod_response_node_t* head_mod_response = NULL; 
mod_response_node_t* butt_mod_response = NULL; 

void enqueue_res(mod_response_t *pres) 
{
    mod_response_node_t *newnode = malloc(sizeof(mod_response_node_t));
    newnode->res = pres;
    newnode->next = NULL;
    if (butt_mod_response == NULL)
        head_mod_response = newnode;
    else 
        butt_mod_response->next = newnode;
    
    butt_mod_response = newnode;
}

mod_response_t *dequeue_res() 
{
    if (head_mod_response == NULL)
        return NULL;

    mod_response_t *result = head_mod_response->res;
    mod_response_node_t *temp = head_mod_response; 
    head_mod_response = head_mod_response->next;
    if (head_mod_response == NULL)
        butt_mod_response = NULL;
    free(temp);
    return result; 
}