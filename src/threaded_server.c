#define DEBUG 1

#include <stdio.h>
#include <string.h>
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


#include "os-challenge-util.h"
#include "server.h"
#include "brute_force.h"
#include "simple_queue.h"

#include "threaded_server.h"



pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond_var = PTHREAD_COND_INITIALIZER;
int food = 0; 


void* brute_force_SHA_threaded_v1(void* p_conn_sd);
void* brute_force_SHA_threaded_pool_busy_worker();
void* brute_force_SHA_threaded_pool_worker();



void launch_thread_per_client_server(struct Server *server)
{
    int conn_sd, socklen;
    struct sockaddr_in cli_addr;
    socklen = sizeof(cli_addr);

    for (;;) {
        if ((conn_sd = accept(server->socket, (struct sockaddr *)&cli_addr, &socklen)) < 0) {
            printf("[server][!] accept() failed \n");
            close(conn_sd);
        }

        pthread_t t;
        pthread_create(&t, NULL, brute_force_SHA_threaded_v1, &conn_sd);
    }
}

void launch_thread_per_client_innate_prio_server(struct Server *server)
{
    int conn_sd, socklen;
    struct sockaddr_in cli_addr;
    socklen = sizeof(cli_addr);

    for (;;) {
        if ((conn_sd = accept(server->socket, (struct sockaddr *)&cli_addr, &socklen)) < 0) {
            printf("[server][!] accept() failed \n");
            close(conn_sd);
        }
        // TODO: set the prio of the threads
        pthread_t t;
        pthread_create(&t, NULL, brute_force_SHA_threaded_v1, &conn_sd);
    }
}

// TODO: add thread_pool size as a parameter for testing? Could be easier
void launch_thread_pool_busy_wait_server(struct Server *server)
{
    int conn_sd, socklen;
    struct sockaddr_in cli_addr;
    socklen = sizeof(cli_addr);

    pthread_t thread_pool[4];
    for (int i = 0; i < 4; ++i)
        pthread_create(&thread_pool[i], NULL, brute_force_SHA_threaded_pool_busy_worker, NULL);

    for (;;) {
        if ((conn_sd = accept(server->socket, (struct sockaddr *)&cli_addr, &socklen)) < 0) {
            printf("[server][!] accept() failed \n");
            close(conn_sd);
        }

        int *pconn = malloc(sizeof(int));
        *pconn = conn_sd;

        pthread_mutex_lock(&queue_mutex);
        enqueue(pconn);
        pthread_mutex_unlock(&queue_mutex);
    }
}

void launch_thread_pool_server(struct Server *server)
{
    int conn_sd, socklen;
    struct sockaddr_in cli_addr;
    socklen = sizeof(cli_addr);

    pthread_t thread_pool[4];
    for (int i = 0; i < 4; ++i)
        pthread_create(&thread_pool[i], NULL, brute_force_SHA_threaded_pool_worker, NULL);

    for (;;) {
        if ((conn_sd = accept(server->socket, (struct sockaddr *)&cli_addr, &socklen)) < 0) {
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



void* brute_force_SHA_threaded_v1(void *pconn_sd)
{
    int conn_sd = *((int*)pconn_sd);
    free(pconn_sd);

    uint8_t buffer[PACKET_REQUEST_SIZE];

    // Read request from client
    bzero(buffer, PACKET_REQUEST_SIZE);
    if ((read(conn_sd, buffer, PACKET_REQUEST_SIZE)) == -1) {
        perror("[server] read() failed");
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
        if (hashMatches(req.hash, guessHash)) {
            res.num = htobe64(res.num);
            break;
        }
    }

    // Write response to client
    if ((write(conn_sd, res.bytes, sizeof(uint64_t))) == -1) {
        perror("[server] write() failed");
        exit(-1);
    }

}

void* brute_force_SHA_threaded_pool_busy_worker()
{
    int *pconn_sd;
    pthread_detach(pthread_self());

    for (;;) {
        pthread_mutex_lock(&queue_mutex);
        pconn_sd = dequeue();
        pthread_mutex_unlock(&queue_mutex);

        if (pconn_sd != NULL)
            brute_force_SHA_threaded_v1(pconn_sd);
    }
}

void* brute_force_SHA_threaded_pool_worker()
{
    int *pconn_sd;
    pthread_detach(pthread_self());
    
    for (;;) {
        pthread_mutex_lock(&queue_mutex);
        while ((pconn_sd = dequeue()) == NULL) {
            //printf("\tthread %ld is waiting\n", pthread_self());
            pthread_cond_wait(&queue_cond_var, &queue_mutex); // check that second parameter is what you think it is
        }
        pthread_mutex_unlock(&queue_mutex);

        brute_force_SHA_threaded_v1(pconn_sd);
    }
}



//void task_splitter(re)