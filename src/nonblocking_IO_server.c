#define _GNU_SOURCE 
#define DEBUG 0

#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <openssl/sha.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <math.h>

#include "os-challenge-util.h"
#include "server.h"
#include "brute_force.h"
#include "threaded_server.h"
#include "simple_queue.h"
#include "os-challenge-util.h"
#include "cached_server.h"
#include "split_request_server.h"
#include "nonblocking_IO_server.h"


#define MAX_EVENTS      5


static void epoll_ctl_add(int epoll_fd, int fd, uint32_t events)
{
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = events;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		perror("[server][!] epoll_ctl() failed ");
		exit(-1);
	}
}


void launch_nonblocking_IO_cached_thread_pool_server(struct Server *server, int nthreads)
{
    int conn_sd, socklen, epoll_fd;
    int nfds;
    struct epoll_event events[MAX_EVENTS];
    unsigned int chunk;
    struct sockaddr_in cli_addr;
    socklen = sizeof(cli_addr);
    uint8_t buffer[PACKET_REQUEST_SIZE];
    request_t req;
    mod_response_t *pres;

    pthread_mutex_init(&queue_mutex, NULL);
    pthread_mutex_init(&done_queue_mutex, NULL);
    pthread_cond_init(&queue_cond_var, NULL);
    
    // Epoll create
    if ((epoll_fd = epoll_create1(0)) == -1) {
        perror("[server][!] epoll_create1() failed");
        close(server->socketfd);
        exit(-1);
    }

    epoll_ctl_add(epoll_fd, server->socketfd, EPOLLIN);

    // Create thread pool
    pthread_t thread_pool[nthreads];
    for (int i = 0; i < nthreads; ++i)
        pthread_create(&thread_pool[i], NULL, thread_pool_nonblocking_IO_cached_worker, NULL);

    // Hash table create
    ht = create_htable();


    // Server loop
    for (;;) {
        if ((nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, 0)) == -1) {
            perror("[server][!] epoll_wait() failed");
            close(server->socketfd);
            exit(-1);
        }

        // Iterate monitored sockets
        for (int i = 0; i < nfds; ++i) {
            int sd = events[i].data.fd;
            if (sd == server->socketfd) {
                // Handle new connection
                if ((conn_sd = accept(server->socketfd, (struct sockaddr *)&cli_addr, &socklen)) < 0) {
                    perror("[server][!] accept() failed");
                    close(conn_sd);
                } 

                #if DEBUG 
                    inet_ntop(AF_INET, (char *)&(cli_addr.sin_addr), buffer, sizeof(cli_addr));
                    printf("[server][+] Connected with %s:%d\n", buffer, ntohs(cli_addr.sin_port));
                #endif

                epoll_ctl_add(epoll_fd, conn_sd, EPOLLIN);
            } 
            else if (events[i].events & EPOLLIN) {
                // Handle EPOLLIN event
                bzero(buffer, PACKET_REQUEST_SIZE);
                if ((read(sd, buffer, PACKET_REQUEST_SIZE)) == -1) {
                    perror("[server][!] read() failed");
                    exit(-1);
                }
            
                // Extrat data from request/msg
                memcpy(&req.hash, buffer + PACKET_REQUEST_HASH_OFFSET, SHA256_DIGEST_LENGTH);

                // Check hashmap, send if cached
                if (htable_contains_key(ht, req.hash)) { 
                    pthread_mutex_lock(&htable_mutex);
                        uint64_t ans = htable_get(ht, req.hash);
                    pthread_mutex_unlock(&htable_mutex);
                    if ((write(conn_sd, &ans, sizeof(uint64_t))) == -1) {
                        perror("[server][!] write() failed");
                        exit(-1);
                    }

                    close(sd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sd, NULL);
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

                    printf("[server][?] start: %lu\n", req.start);
                    printf("[server][?] end:   %lu\n", req.end);
                    printf("[server][?] p:     %u\n", req.prio);
                #endif 

                sumbit_task(sd, req.hash, req.start, req.end);

                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sd, NULL);
            }
        }

        // Write back to client
        pthread_mutex_lock(&done_queue_mutex);
            while ((pres = dequeue_res()) != NULL) {
                pthread_mutex_unlock(&done_queue_mutex);

                    if ((write(pres->sd, pres->reverse_hash.bytes, sizeof(uint64_t))) == -1) {
                        perror("[server][!] write() failed");
                        exit(-1);
                    }

                    close(pres->sd);
                    free(pres);
                pthread_mutex_lock(&done_queue_mutex);
            }
        pthread_mutex_unlock(&done_queue_mutex);
    }
}


void launch_nonblocking_IO_cached_split_thread_pool_server(struct Server *server, int nthreads)
{
    int conn_sd, socklen, epoll_fd;
    int nfds;
    struct epoll_event events[MAX_EVENTS];
    int req_id = 0;
    unsigned int chunk;
    struct sockaddr_in cli_addr;
    socklen = sizeof(cli_addr);
    uint8_t buffer[PACKET_REQUEST_SIZE];
    request_t req;
    mod_response_t *pres;

    pthread_mutex_init(&queue_mutex, NULL);
    pthread_mutex_init(&done_queue_mutex, NULL);
    pthread_cond_init(&queue_cond_var, NULL);
    
    // Epoll create
    if ((epoll_fd = epoll_create1(0)) == -1) {
        perror("[server][!] epoll_create1() failed");
        close(server->socketfd);
        exit(-1);
    }

    epoll_ctl_add(epoll_fd, server->socketfd, EPOLLIN);

    // Create thread pool
    pthread_t thread_pool[nthreads];
    for (int i = 0; i < nthreads; ++i)
        pthread_create(&thread_pool[i], NULL, thread_pool_nonblocking_IO_cached_split_worker, NULL);

    // Hash table create
    ht = create_htable();


    // Server loop
    for (;;) {
        if ((nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, 0)) == -1) {
            perror("[server][!] epoll_wait() failed");
            close(server->socketfd);
            exit(-1);
        }

        // Iterate monitored sockets
        for (int i = 0; i < nfds; ++i) {
            int sd = events[i].data.fd;
            if (sd == server->socketfd) {
                // Handle new connection
                if ((conn_sd = accept(server->socketfd, (struct sockaddr *)&cli_addr, &socklen)) < 0) {
                    perror("[server][!] accept() failed");
                    close(conn_sd);
                } 

                #if DEBUG 
                    inet_ntop(AF_INET, (char *)&(cli_addr.sin_addr), buffer, sizeof(cli_addr));
                    printf("[server][+] Connected with %s:%d\n", buffer, ntohs(cli_addr.sin_port));
                #endif

                epoll_ctl_add(epoll_fd, conn_sd, EPOLLIN);
            } 
            else if (events[i].events & EPOLLIN) {
                // Handle EPOLLIN event
                bzero(buffer, PACKET_REQUEST_SIZE);
                if ((read(sd, buffer, PACKET_REQUEST_SIZE)) == -1) {
                    perror("[server][!] read() failed");
                    exit(-1);
                }
            
                // Extrat data from request/msg
                memcpy(&req.hash, buffer + PACKET_REQUEST_HASH_OFFSET, SHA256_DIGEST_LENGTH);

                // Check hashmap, send if cached
                if (htable_contains_key(ht, req.hash)) { 
                    pthread_mutex_lock(&htable_mutex);
                        uint64_t ans = htable_get(ht, req.hash);
                    pthread_mutex_unlock(&htable_mutex);
                    if ((write(conn_sd, &ans, sizeof(uint64_t))) == -1) {
                        perror("[server][!] write() failed");
                        exit(-1);
                    }

                    close(sd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sd, NULL);
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

                    printf("[server][?] start: %lu\n", req.start);
                    printf("[server][?] end:   %lu\n", req.end);
                    printf("[server][?] p:     %u\n", req.prio);
                #endif 

                split_and_sumbit_task(nthreads, sd, req_id++, req.hash, req.start, req.end);

                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sd, NULL);
            }
        }

        // Write back to client
        pthread_mutex_lock(&done_queue_mutex);
            while ((pres = dequeue_res()) != NULL) {
                pthread_mutex_unlock(&done_queue_mutex);

                    if ((write(pres->sd, pres->reverse_hash.bytes, sizeof(uint64_t))) == -1) {
                        perror("[server][!] write() failed");
                        exit(-1);
                    }

                    close(pres->sd);
                    free(pres);
                pthread_mutex_lock(&done_queue_mutex);
            }
        pthread_mutex_unlock(&done_queue_mutex);
    }
}



void *thread_pool_nonblocking_IO_cached_worker()
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

        mod_response_t res = {0};
        res.sd = task.sd;
        for (res.reverse_hash.num = task.start; res.reverse_hash.num < task.end; ++res.reverse_hash.num) {
            SHA256(res.reverse_hash.bytes, sizeof(uint64_t), guess_hash);
            if (memcmp(task.hash, guess_hash, SHA256_DIGEST_LENGTH) == 0) {
                res.reverse_hash.num = htobe64(res.reverse_hash.num);

                mod_response_t *pres = malloc(sizeof(response_t));
                *pres = res;

                pthread_mutex_lock(&done_queue_mutex);
                    enqueue_res(pres);
                pthread_mutex_unlock(&done_queue_mutex);

                pthread_mutex_lock(&htable_mutex);
                    htable_set(ht, guess_hash, res.reverse_hash.num);
                pthread_mutex_unlock(&htable_mutex);

                break;
            }
        }
    }
}


void *thread_pool_nonblocking_IO_cached_split_worker()
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

        mod_response_t res = {0};
        res.sd = task.sd;
        for (res.reverse_hash.num = task.start; res.reverse_hash.num < task.end; ++res.reverse_hash.num) {
            if (done_board[task.id])
                break;


            SHA256(res.reverse_hash.bytes, sizeof(uint64_t), guess_hash);
            if (memcmp(task.hash, guess_hash, SHA256_DIGEST_LENGTH) == 0) {
                done_board[task.id] = true; 
                res.reverse_hash.num = htobe64(res.reverse_hash.num);

                mod_response_t *pres = malloc(sizeof(response_t));
                *pres = res;

                pthread_mutex_lock(&done_queue_mutex);
                    enqueue_res(pres);
                pthread_mutex_unlock(&done_queue_mutex);

                pthread_mutex_lock(&htable_mutex);
                    htable_set(ht, guess_hash, res.reverse_hash.num);
                pthread_mutex_unlock(&htable_mutex);

                break;
            }
        }
    }
}