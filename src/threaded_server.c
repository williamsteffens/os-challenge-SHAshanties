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


void* brute_force_SHA_threaded(void* pconn_sd);
void* brute_force_SHA_thread_pool_busy_worker();
void* brute_force_SHA_thread_pool_worker();
void* thread_pool_worker();
void* worker_thread();



void launch_thread_per_client_server(struct Server *server)
{
    int conn_sd, socklen;
    struct sockaddr_in cli_addr;
    socklen = sizeof(cli_addr);

    for (;;) {
        if ((conn_sd = accept(server->socketfd, (struct sockaddr *)&cli_addr, &socklen)) < 0) {
            printf("[server][!] accept() failed \n");
            close(conn_sd);
        }

        int *pconn = malloc(sizeof(int));
        *pconn = conn_sd;

        pthread_t t;
        pthread_create(&t, NULL, brute_force_SHA_threaded, pconn);
    }
}

void launch_thread_pool_server(struct Server *server, int nthreads)
{
    int conn_sd, socklen;
    struct sockaddr_in cli_addr;
    socklen = sizeof(cli_addr);

    pthread_mutex_init(&queue_mutex, NULL);
    pthread_cond_init(&queue_cond_var, NULL);

    pthread_t thread_pool[nthreads];
    for (int i = 0; i < nthreads; ++i)
        pthread_create(&thread_pool[i], NULL, brute_force_SHA_thread_pool_worker, NULL);

    for (;;) {
        if ((conn_sd = accept(server->socketfd, (struct sockaddr *)&cli_addr, &socklen)) < 0) {
            perror("[server][!] accept() failed \n");
            close(conn_sd);
        }

        int *pconn = malloc(sizeof(int));
        *pconn = conn_sd;

        pthread_mutex_lock(&queue_mutex);
            enqueue(pconn);
        pthread_cond_signal(&queue_cond_var);
        pthread_mutex_unlock(&queue_mutex);
    }
}

void *thread_pool_worker()
{
    pthread_detach(pthread_self());
    task_t *ptask;
    task_t task;
    uint8_t guess_hash[SHA256_DIGEST_LENGTH];

    for (;;) {
        pthread_mutex_lock(&queue_mutex);
            while ((ptask = dequeue_task()) == NULL)
                pthread_cond_wait(&queue_cond_var, &queue_mutex);
        pthread_mutex_unlock(&queue_mutex);

        // Busy waiting
        // pthread_mutex_lock(&queue_mutex);
        //     while ((ptask = dequeue_task()) == NULL) {
        //         pthread_mutex_unlock(&queue_mutex);
        //         pthread_mutex_lock(&queue_mutex);
        //     }
        // pthread_mutex_unlock(&queue_mutex);

        task_t task = *ptask;
        free(ptask);

        #if DEBUG
            printf("task hash: ");
            for (int i = SHA256_DIGEST_LENGTH - 1; i >= 0; --i)
                printf("%02x", task.hash[i]);
            printf("\n");

            printf("task start: %lu\n", task.start);
            printf("task end:   %lu\n", task.end);
        #endif

        response_t res = {0};
        for (res.num = task.start; res.num < task.end; ++res.num) {
            if (done_board[task.id])
                break;

            SHA256(res.bytes, sizeof(uint64_t), guess_hash);
            if (memcmp(task.hash, guess_hash, SHA256_DIGEST_LENGTH) == 0) {
                res.num = htobe64(res.num);
                if ((write(task.sd, res.bytes, sizeof(uint64_t))) == -1) {
                    perror("[server][!] write() failed\n");
                    exit(-1);
                }

                done_board[task.id] = true; 
                close(task.sd);
                break;
            }
        }
    }
}













void* brute_force_SHA_threaded(void *pconn_sd)
{
    int conn_sd = *((int*)pconn_sd);
    free(pconn_sd);

    uint8_t buffer[PACKET_REQUEST_SIZE];

    bzero(buffer, PACKET_REQUEST_SIZE);
    if ((read(conn_sd, buffer, PACKET_REQUEST_SIZE)) == -1) {
        perror("[server][!] read() failed");
        exit(-1);
    }

    // Zerolize vars
    request_t req = {0};

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

    // Brute force
    uint8_t guessHash[SHA256_DIGEST_LENGTH];
    response_t res = {0};
    for (res.num = req.start; res.num < req.end; ++res.num) {
        SHA256(res.bytes, sizeof(uint64_t), guessHash);
        if (memcmp(req.hash, guessHash, SHA256_DIGEST_LENGTH) == 0) {
            res.num = htobe64(res.num);

            // Write response to client
            if ((write(conn_sd, res.bytes, sizeof(uint64_t))) == -1) {
                perror("[server] write() failed");
                exit(-1);
            }
            
            break;
        }
    }

    // Close the connection when done
    close(conn_sd);
}

void* brute_force_SHA_thread_pool_worker()
{
    int *pconn_sd;
    pthread_detach(pthread_self());
    
    for (;;) {
        pthread_mutex_lock(&queue_mutex);
            while ((pconn_sd = dequeue()) == NULL) {
                pthread_cond_wait(&queue_cond_var, &queue_mutex);
            }
        pthread_mutex_unlock(&queue_mutex);

        brute_force_SHA_threaded(pconn_sd);
    }
}




