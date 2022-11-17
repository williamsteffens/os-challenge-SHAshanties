#include <stdio.h>
#include <string.h>
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


#include "os-challenge-util.h"
#include "server.h"
#include "brute_force.h"
#include "simple_queue.h"
#include "hash_table.h"
#include <pthread.h>
#include <sched.h>


pthread_mutex_t queue_mutex_cached = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t htable_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond_var_cached = PTHREAD_COND_INITIALIZER;

bool done_board[1000];
htable_t *ht;

void* thread_pool_worker_cached();


void launch_cached_split_req_thread_pool_server(struct Server *server, int nthreads)
{
    int conn_sd, socklen;
    int req_id = 0;
    unsigned int chunk;
    struct sockaddr_in cli_addr;
    socklen = sizeof(cli_addr);
    uint8_t buffer[PACKET_REQUEST_SIZE];
    request_t req;

    // Set pthread attributes
    // pthread_attr_t attr;
    // int newprio = 99;
    // sched_param param;
    // int ret;
    // ret = pthread_attr_init (&attr);
    // ret = pthread_attr_getschedparam (&attr, &param);
    // param.sched_priority = newprio;
    // ret = pthread_attr_setschedparam (&attr, &param);

    // Create Thread Pool
    pthread_t thread_pool[nthreads];
    for (int i = 0; i < nthreads; ++i)
        pthread_create(&thread_pool[i], NULL, thread_pool_worker_cached, NULL);

    ht = create_htable();


    // Server loop
    for (;;) {  
        if ((conn_sd = accept(server->socketfd, (struct sockaddr *)&cli_addr, &socklen)) < 0) {
            perror("[server][!] accept() failed");
            close(conn_sd);
        } 

        #if DEBUG 
            inet_ntop(AF_INET, (char *)&(cli_addr.sin_addr), buffer, sizeof(cli_addr));
            printf("[server][+] Connected with %s:%d\n", buffer, ntohs(cli_addr.sin_port));
        #endif
    
        bzero(buffer, PACKET_REQUEST_SIZE);
        if ((read(conn_sd, buffer, PACKET_REQUEST_SIZE)) == -1) {
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

            continue; 
        }

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

        chunk = 1 + (((req.end - req.start) - 1) / nthreads); // ceil(range/nthreads)-ish --- NOTE THE -ISH

        for (int i = 0; i < nthreads - 1; ++i) {
            task_t *ptask = malloc(sizeof(task_t));
            ptask->id     = req_id;
            ptask->sd     = conn_sd;
            memcpy(&ptask->hash, req.hash, SHA256_DIGEST_LENGTH);
            ptask->start  = req.start + i * chunk;
            ptask->end    = req.start + (i + 1) * chunk;

            // Enqueue the split request
            pthread_mutex_lock(&queue_mutex_cached);
                enqueue_task(ptask);
            pthread_cond_signal(&queue_cond_var_cached);
            pthread_mutex_unlock(&queue_mutex_cached);
        }
        task_t *ptask = malloc(sizeof(task_t));
        ptask->id     = req_id;
        ptask->sd     = conn_sd;
        memcpy(&ptask->hash, req.hash, SHA256_DIGEST_LENGTH);
        ptask->start  = req.start + (nthreads - 1) * chunk;
        ptask->end    = req.end; 

        // Enqueue the split request
        pthread_mutex_lock(&queue_mutex_cached);
            enqueue_task(ptask);
        pthread_cond_signal(&queue_cond_var_cached);
        pthread_mutex_unlock(&queue_mutex_cached); 

        // Inc req_id for next request
        ++req_id;
    }
}

void* thread_pool_worker_cached()
{
    pthread_detach(pthread_self());
    task_t *ptask;
    task_t task;
    uint8_t guess_hash[SHA256_DIGEST_LENGTH];

    for (;;) {
        pthread_mutex_lock(&queue_mutex_cached);
            while ((ptask = dequeue_task()) == NULL)
                pthread_cond_wait(&queue_cond_var_cached, &queue_mutex_cached);
        pthread_mutex_unlock(&queue_mutex_cached);

        // Busy waiting
        // pthread_mutex_lock(&queue_mutex_cached);
        //     while ((ptask = dequeue_task()) == NULL) {
        //         pthread_mutex_unlock(&queue_mutex_cached);
        //         pthread_mutex_lock(&queue_mutex_cached);
        //     }
        // pthread_mutex_unlock(&queue_mutex_cached);

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
