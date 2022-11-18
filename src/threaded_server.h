#pragma once

#include "os-challenge-util.h"
#include "hash_table.h"


pthread_mutex_t queue_mutex;
pthread_cond_t queue_cond_var;


bool done_board[1000];


void launch_thread_per_client_server(struct Server *server);

void launch_thread_pool_server(struct Server *server, int nthreads);

void launch_split_req_thread_pool_server(struct Server *server, int nthreads);


void *thread_pool_worker();