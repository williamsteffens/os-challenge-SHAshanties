#define _GNU_SOURCE 
#define DEBUG 0

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/epoll.h>
#include <openssl/sha.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>

#include "os-challenge-util.h"
#include "server.h"
#include "brute_force.h"
#include "simple_queue.h"
#include "cached_server.h"

#include <semaphore.h>

#include "threaded_server.h"
#include "split_request_server.h"



void launch_split_req_cached_thread_pool_server(struct Server *server, int nthreads)
{
    int conn_sd, socklen;
    int req_id = 0;
    struct sockaddr_in cli_addr;
    socklen = sizeof(cli_addr);
    uint8_t buffer[PACKET_REQUEST_SIZE];
    request_t req;

    pthread_mutex_init(&queue_mutex, NULL);
    pthread_cond_init(&queue_cond_var, NULL);

    ht = create_htable();

    pthread_t thread_pool[nthreads];
    for (int i = 0; i < nthreads; ++i) {
        int *arg = malloc(sizeof(int));
        *arg = i;
        pthread_create(&thread_pool[i], NULL, thread_pool_worker_split_cached, arg);
    }


    for (;;) {
        if ((conn_sd = accept(server->socketfd, (struct sockaddr *)&cli_addr, &socklen)) < 0) {
            perror("[server][!] accept() failed \n");
            close(conn_sd);
        }

        bzero(buffer, PACKET_REQUEST_SIZE);
        if ((read(conn_sd, buffer, PACKET_REQUEST_SIZE)) == -1) {
            perror("[server][!] read() failed");
            exit(-1);
        }

        // Extrat data from request/msg
        memcpy(&req.hash, buffer + PACKET_REQUEST_HASH_OFFSET, SHA256_DIGEST_LENGTH);

        // Check hashmap, send if cached
        if (htable_contains_key(ht, req.hash)) { 
            uint64_t ans = htable_get(ht, req.hash);
            if ((write(conn_sd, &ans, sizeof(uint64_t))) == -1) {
                perror("[server][!] write() failed");
                exit(-1);
            }

            continue; 
        }

        memcpy(&req.start, buffer + PACKET_REQUEST_START_OFFSET, 8);
        memcpy(&req.end, buffer + PACKET_REQUEST_END_OFFSET, 8);
        req.prio = buffer[PACKET_REQUEST_PRIO_OFFSET];

        // Endianness 
        req.start = be64toh(req.start);
        req.end = be64toh(req.end);

        #if DEBUG
            printf("[server][?] hash: ");
            for (int i = SHA256_DIGEST_LENGTH - 1; i >= 0; --i)
                printf("%02x", req.hash[i]);
            printf("\n");

            printf("[server][?] start: %d\n", (int)req.start);
            printf("[server][?] end:   %d\n", (int)req.end);
            printf("[server][?] p:     %u\n", req.prio);
        #endif 

        req.sd = conn_sd; 
        req.id = req_id++;

        split_and_sumbit_task(nthreads, conn_sd, req.id, req.hash, req.start, req.end);
    }
}


void *thread_pool_worker_split_cached(void *arg)
{
    int num = *(int*)arg;
    free(arg);
    pthread_detach(pthread_self());
    task_t *ptask;
    task_t task;
    uint8_t guess_hash[SHA256_DIGEST_LENGTH];

    for (;;) {
        pthread_mutex_lock(&queue_mutex);
            while ((ptask = dequeue_task()) == NULL)
                pthread_cond_wait(&queue_cond_var, &queue_mutex);
        pthread_mutex_unlock(&queue_mutex);

        task = *ptask;
        free(ptask);

        #if DEBUG
            printf("[server][?] task hash: ");
            for (int i = SHA256_DIGEST_LENGTH - 1; i >= 0; --i)
                printf("%02x", task.hash[i]);
            printf("\n");

            printf("[server][?] task start: %lu\n", task.start);
            printf("[server][?] task end:   %lu\n", task.end);
        #endif

        response_t res = {0};
        for (res.num = task.start; res.num < task.end; ++res.num) {
            if (done_board[task.id])
                break;

            SHA256(res.bytes, sizeof(uint64_t), guess_hash);
            if (memcmp(task.hash, guess_hash, SHA256_DIGEST_LENGTH) == 0) {
                res.num = htobe64(res.num);

                done_board[task.id] = true;

                if ((write(task.sd, res.bytes, sizeof(uint64_t))) == -1) {
                    perror("[server][!] write() failed\n");
                    exit(-1);
                }

                pthread_mutex_lock(&htable_mutex);
                    htable_set(ht, guess_hash, res.num);
                pthread_mutex_unlock(&htable_mutex);

                close(task.sd);
                break;
            }
        }
    }
}


void sumbit_split_task(int sd, int id, uint8_t hash[SHA256_DIGEST_LENGTH], uint64_t start, uint64_t end)
{
    task_t *ptask = malloc(sizeof(task_t));
    ptask->sd = sd; 
    ptask->id = id;
    memcpy(&ptask->hash, hash, SHA256_DIGEST_LENGTH);
    ptask->start = start;
    ptask->end = end;

    pthread_mutex_lock(&queue_mutex);
        enqueue_task(ptask);
    pthread_cond_signal(&queue_cond_var);
    pthread_mutex_unlock(&queue_mutex);
}


void split_and_sumbit_task(int nthreads, int sd, int id, uint8_t hash[SHA256_DIGEST_LENGTH], uint64_t start, uint64_t end)
{
    unsigned int chunk = ceil((end - start) / nthreads);

    for (int i = 0; i < nthreads - 1; ++i)
        sumbit_split_task(sd, id, hash, start + i * chunk, start + (i + 1) * chunk);

    sumbit_split_task(sd, id, hash, start + (nthreads - 1) * chunk, end);
}