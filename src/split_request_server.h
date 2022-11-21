#pragma once

#include "os-challenge-util.h"
#include "hash_table.h"
#include <semaphore.h>

bool done_board[1000];

pthread_mutex_t req_queue_mutex;
pthread_cond_t req_queue_cond_var;

void *thread_pool_worker_split_cached();
void *task_distributor();
void sumbit_split_task(int sd, int id, uint8_t hash[SHA256_DIGEST_LENGTH], uint64_t start, uint64_t end);
void split_and_sumbit_task(int nthreads, int sd, int id, uint8_t hash[SHA256_DIGEST_LENGTH], uint64_t start, uint64_t end);