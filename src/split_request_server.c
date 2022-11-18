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

#include "threaded_server.h"
#include "split_request_server.h"



static void sumbit_task(int sd, int id, uint8_t hash[SHA256_DIGEST_LENGTH], uint64_t start, uint64_t end)
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
        sumbit_task(sd, id, hash, start + i * chunk, start + (i + 1) * chunk);

    sumbit_task(sd, id, hash, start + (nthreads - 1) * chunk, end);
}


void launch_split_req_thread_pool_server(struct Server *server, int nthreads)
{
    int conn_sd, socklen;
    int req_id = 0;
    struct sockaddr_in cli_addr;
    socklen = sizeof(cli_addr);
    uint8_t buffer[PACKET_REQUEST_SIZE];
    request_t req;

    pthread_mutex_init(&queue_mutex, NULL);
    pthread_cond_init(&queue_cond_var, NULL);

    pthread_t thread_pool[nthreads];
    for (int i = 0; i < nthreads; ++i)
        pthread_create(&thread_pool[i], NULL, thread_pool_worker, NULL);

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
        memcpy(&req.start, buffer + PACKET_REQUEST_START_OFFSET, 8);
        memcpy(&req.end, buffer + PACKET_REQUEST_END_OFFSET, 8);
        req.prio = buffer[PACKET_REQUEST_PRIO_OFFSET];

        // Endianness 
        req.start = be64toh(req.start);
        req.end = be64toh(req.end);

        #if DEBUG
            printf("hash: ");
            for (int i = SHA256_DIGEST_LENGTH - 1; i >= 0; --i)
                printf("%02x", req.hash[i]);
            printf("\n");

            printf("start: %d\n", (int)req.start);
            printf("end:   %d\n", (int)req.end);
            printf("p:     %u\n", req.prio);
        #endif 

        split_and_sumbit_task(nthreads, conn_sd, req_id, req.hash, req.start, req.end);

        // Inc req_id for next request
        ++req_id;
    }
}
