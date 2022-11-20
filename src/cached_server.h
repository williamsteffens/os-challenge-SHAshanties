#pragma once 

#include "hash_table.h"



pthread_mutex_t htable_mutex;

htable_t *ht;



void *thread_pool_worker_cached();


void launch_cached_split_req_thread_pool_server(struct Server *server, int nthreads);