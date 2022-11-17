#pragma once

#include "thread_pool.h"
#include "os-challenge-util.h"
#include "hash_table.h"



typedef struct bf_task {
    bool *done; 
    uint8_t hash[SHA256_DIGEST_LENGTH];
    int sd;
    uint64_t start;
    uint64_t end;
    htable_t *ht;
} bf_task_t;


void launch_thread_per_client_server(struct Server *server);
void launch_thread_pool_server(struct Server *server, int nthreads);
void launch_split_req_thread_pool_server(struct Server *server, int nthreads);



void launch_thread_per_client_innate_prio_server(struct Server *server);
void launch_thread_pool_busy_wait_server(struct Server *server);
void launch_nthreads_one_client_server(struct Server *server, int nthreads);
void launch_nthreads_one_client_server_many_tasks_server(struct Server *server, int nthreads);