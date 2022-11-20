#pragma once 

#include "task_queue.h"
#include "task_priority_queue.h"

task_priority_queue_t *pq;

void launch_priority_cached_thread_pool_server(struct Server *server, int nthreads);

void *thread_pool_worker_priority_cached();


void sumbit_priority_task(int sd, uint8_t hash[SHA256_DIGEST_LENGTH], uint64_t start, uint64_t end, uint8_t prio, task_priority_queue_t *pq);