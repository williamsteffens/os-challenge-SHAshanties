#pragma once

#include "os-challenge-util.h"
#include "hash_table.h"

void split_and_sumbit_task(int nthreads, int sd, int id, uint8_t hash[SHA256_DIGEST_LENGTH], uint64_t start, uint64_t end);