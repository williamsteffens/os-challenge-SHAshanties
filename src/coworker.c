#define DEBUG 0

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include "os-challenge-util.h"
#include "server.h"
#include "hash_table.h"


typedef struct setup {
    int offset;
    struct Server *server;
} setup_t;

pthread_mutex_t accept_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condvar = PTHREAD_COND_INITIALIZER;

request_t *req_board[1000];
int totalthreads;

htable_t *ht;


void* co_worker(void *arg)
{
    // Unpack and free setup
    setup_t *setup = (setup_t*)arg;
    int offset = setup->offset;
    struct Server *server = setup->server;
    free(setup);

    int conn_sd, socklen;
    struct sockaddr_in cli_addr;
    socklen = sizeof(cli_addr);
    int cnt = 0;
    uint64_t start, end;
    uint8_t buffer[PACKET_REQUEST_SIZE];
    uint8_t guess_hash[SHA256_DIGEST_LENGTH];
    request_t req;
    response_t res;

    // Server loop
    for (;;) {
        // accept() req and put up on req board
        pthread_mutex_lock(&accept_mutex);
            while (req_board[cnt] == NULL) {
                if ((conn_sd = accept(server->socketfd, (struct sockaddr *)&cli_addr, &socklen)) < 0) {
                    perror("[server][!] accept() failed");
                    close(conn_sd);
                    exit(-1);
                }

                // Read request from client
                bzero(buffer, PACKET_REQUEST_SIZE);
                if ((read(conn_sd, buffer, PACKET_REQUEST_SIZE)) == -1) {
                    perror("[server][!] read() failed");
                    exit(-1);
                }

                // Extrat data from req
                memcpy(&req.hash, buffer + PACKET_REQUEST_HASH_OFFSET, SHA256_DIGEST_LENGTH);
                if (htable_contains_key(ht, req.hash)) {
                    uint64_t ans = htable_get(ht, req.hash);
                    if ((write(conn_sd, &ans, sizeof(uint64_t))) == -1) {
                        perror("[server][!] write() failed");
                        exit(-1);
                    }

                    continue;
                } 
                // else {
                //     // SPAWN MONKE
                //     htable_set
                // }

                memcpy(&req.start, buffer + PACKET_REQUEST_START_OFFSET, 8);
                memcpy(&req.end, buffer + PACKET_REQUEST_END_OFFSET, 8);
                req.prio = buffer[PACKET_REQUEST_PRIO_OFFSET];
                req.sd = conn_sd;

                // Endianness 
                req.start = be64toh(req.start);
                req.end = be64toh(req.end);

                // Put req up on req board
                request_t *preq = malloc(sizeof(request_t));
                *preq = req;
                req_board[cnt] = preq;
            }

            // Take local copy of req
            req = *req_board[cnt];

            #if DEBUG
                printf("hash: ");
                for (int i = SHA256_DIGEST_LENGTH - 1; i >= 0; --i)
                    printf("%02x", req.hash[i]);
                printf("\n");

                printf("start: %d\n", (int)req.start);
                printf("end:   %d\n", (int)req.end);
                printf("p:     %u\n", req.prio);
            #endif 
        pthread_mutex_unlock(&accept_mutex);
        
        // Define start and end of req based on range
        unsigned int chunk = 1 + (((req.end - req.start) - 1) / totalthreads); // ceil(range/nthreads)-ish --- NOTE THE -ISH        
        
        if (offset < totalthreads - 1) {
            // Not last thread
            start = offset * chunk + req.start; 
            end = (offset + 1) * chunk + req.start; 
        }
        else {
            // Last thread
            start = offset * chunk + req.start;
            end = req.end; 
        }

        // Process req
        for (res.num = start; res.num < end; ++res.num) {
            // Check if the req has been taken down
            if (req_board[cnt]->resolved)
                break;

            // Reverse hash
            SHA256(res.bytes, sizeof(uint64_t), guess_hash);
            if (memcmp(req.hash, guess_hash, SHA256_DIGEST_LENGTH) == 0) {
                res.num = htobe64(res.num);
                if ((write(req.sd, res.bytes, sizeof(uint64_t))) == -1) {
                    perror("[server][!] write() failed");
                    exit(-1);
                }

                // "Take down" the req from the req_board and close the connection
                //pthread_mutex_lock(&accept_mutex);
                    htable_set(ht, guess_hash, res.num);
                //pthread_mutex_unlock(&accept_mutex);
                req_board[cnt]->resolved = true; 
                close(req.sd);
                break;
            }
        }

        ++cnt;
    }
}


void launch_co_worker_server(struct Server *server, int nthreads) 
{
    totalthreads = nthreads;
    ht = create_htable();

    // Create thread pool
    pthread_t thread_pool[nthreads];
    for (int i = 0; i < nthreads; ++i) {
        setup_t *setup = malloc(sizeof(setup_t));
        setup->offset = i;
        setup->server = server;       
        
        if (pthread_create(&thread_pool[i], NULL, co_worker, setup) != 0) {
            perror("[server][!] pthread_create() failed \n");
            exit(-1);
        }
    }

    // Kill the launch thread
    pause();
}



































































void* og_co_worker(void *arg)
{
    // Unpack and free setup
    setup_t *setup = (setup_t*)arg;
    int offset = setup->offset;
    struct Server *server = setup->server;
    free(setup);

    int conn_sd, socklen;
    struct sockaddr_in cli_addr;
    socklen = sizeof(cli_addr);
    int cnt = 0;
    uint64_t start, end;
    uint8_t buffer[PACKET_REQUEST_SIZE];
    uint8_t guess_hash[SHA256_DIGEST_LENGTH];
    request_t req;
    response_t res;

    // Server loop
    for (;;) {
        // accept() req and put up on req board
        pthread_mutex_lock(&accept_mutex);
            if (req_board[cnt] == NULL) {
                if ((conn_sd = accept(server->socketfd, (struct sockaddr *)&cli_addr, &socklen)) < 0) {
                    perror("[server][!] accept() failed");
                    close(conn_sd);
                    exit(-1);
                }

                // Read request from client
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
                req.sd = conn_sd;

                // Endianness 
                req.start = be64toh(req.start);
                req.end = be64toh(req.end);

                // Put req up on req board
                request_t *preq = malloc(sizeof(request_t));
                *preq = req;
                req_board[cnt] = preq;
            }

            // Take local copy of req
            req = *req_board[cnt];

            #if DEBUG
                printf("hash: ");
                for (int i = SHA256_DIGEST_LENGTH - 1; i >= 0; --i)
                    printf("%02x", req.hash[i]);
                printf("\n");

                printf("start: %d\n", (int)req.start);
                printf("end:   %d\n", (int)req.end);
                printf("p:     %u\n", req.prio);
            #endif 
        pthread_mutex_unlock(&accept_mutex);
        
        // Define start and end of req based on range
        unsigned int chunk = 1 + (((req.end - req.start) - 1) / totalthreads); // ceil(range/nthreads)-ish --- NOTE THE -ISH        
        
        if (offset < totalthreads - 1) {
            // Not last thread
            start = offset * chunk + req.start; 
            end = (offset + 1) * chunk + req.start; 
        }
        else {
            // Last thread
            start = offset * chunk + req.start;
            end = req.end; 
        }

        // Process req
        for (res.num = start; res.num < end; ++res.num) {
            // Check if the req has been taken down
            if (req_board[cnt] == NULL)
                break;

            // Reverse hash
            SHA256(res.bytes, sizeof(uint64_t), guess_hash);
            if (memcmp(req.hash, guess_hash, SHA256_DIGEST_LENGTH) == 0) {
                res.num = htobe64(res.num);
                if ((write(req.sd, res.bytes, sizeof(uint64_t))) == -1) {
                    perror("[server][!] write() failed");
                    exit(-1);
                }

                // "Take down" the req from the req_board and close the connection
                pthread_mutex_lock(&accept_mutex);
                    free(req_board[cnt]);
                    req_board[cnt] = NULL;
                pthread_mutex_unlock(&accept_mutex);
                close(req.sd);
                break;
            }
        }

        ++cnt;
    }
}