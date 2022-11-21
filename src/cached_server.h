#pragma once 

#include "hash_table.h"

pthread_mutex_t htable_mutex;
htable_t *ht;

void launch_cached_thread_pool_server(struct Server *server, int nthreads);
void *thread_pool_worker_cached();