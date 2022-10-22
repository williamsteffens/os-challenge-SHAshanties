
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/syscall.h>
#include "server.h"
#include "server_I.h"
#include "os-challenge-util.h"
#include "thread_pool.h"
#include "threaded_server.h"
#include "hash_table.h"
#include "simple_queue.h"



typedef struct init {
    int         id; 
    uint64_t    range_start;
    uint64_t    range_end;
} init_t;


pthread_barrier_t barrier_0;
pthread_barrier_t barrier_1;
pthread_mutex_t queue_mutex_II = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond_var_II = PTHREAD_COND_INITIALIZER;
htable_t *ht;

request_t *shared_req;



long get_range(uint8_t buffer[PACKET_REQUEST_SIZE])
{
    uint64_t start;
    uint64_t end;
    memcpy(&start, buffer + PACKET_REQUEST_START_OFFSET, 8);
    memcpy(&end, buffer + PACKET_REQUEST_END_OFFSET, 8);
    start = be64toh(start);
    end = be64toh(end);

    return end - start;
}

void submit_req(int sd, uint8_t buffer[PACKET_REQUEST_SIZE])
{
    request_t *preq = malloc(sizeof(request_t));
    preq->sd = sd;

    memcpy(&preq->hash, buffer + PACKET_REQUEST_HASH_OFFSET, SHA256_DIGEST_LENGTH);
    memcpy(&preq->start, buffer + PACKET_REQUEST_START_OFFSET, 8);
    memcpy(&preq->end, buffer + PACKET_REQUEST_END_OFFSET, 8);

    preq->start = be64toh(preq->start);
    preq->end = be64toh(preq->end);

    preq->resolved = false; 

    pthread_mutex_lock(&queue_mutex_II);
        enqueue_req(preq);
        pthread_cond_signal(&queue_cond_var_II);
    pthread_mutex_unlock(&queue_mutex_II);
}

void worker(void *arg)
{
    init_t   *setup        = (init_t*)arg;
    int       id           = setup->id;
    uint64_t  range_start  = setup->range_start;
    uint64_t  range_end    = setup->range_end;
    free(setup);

    uint8_t     guess_hash[SHA256_DIGEST_LENGTH];
    request_t  *panic_req;
    response_t  res = {0};
    // TODO: do a lap and if it was not resolved wait?
    unsigned long entered_at = 0; 


    unsigned long num = range_start; 
    for (;;) {
        printf("[server][t:%d] Waiting at barrier_0...\n", id);
        pthread_barrier_wait(&barrier_0);
            
        if (id == 0) {
            // if (*shared_req != NULL) {
            //     free((*shared_req)->resolved);
            //     // TODO: should we free the request here or what? We want to keeps it yes keeps it for ourself!
            //     // maybe a temp pointer free?
            // }
            
            pthread_mutex_lock(&queue_mutex_II);
                while ((shared_req = dequeue_req()) == NULL)
                    pthread_cond_wait(&queue_cond_var_II, &queue_mutex_II);
            pthread_mutex_unlock(&queue_mutex_II);
        }

        printf("[server][t:%d] Waiting at barrier_1...\n", id);
        pthread_barrier_wait(&barrier_1);

        entered_at = num;
    
        // if (panic_mode) {
        //     panic_req
        // }

        // maybe a while loop here? might be the same as just going around in the infinite for loop
        
        
        //printf("%lu, t: %d\n", res.num, id);

        while (!shared_req->resolved) {
            res.num = shared_req->start + num;

            SHA256(res.bytes, sizeof(uint64_t), guess_hash);
            if (memcmp(shared_req->hash, guess_hash, SHA256_DIGEST_LENGTH) == 0) {
                res.num = htobe64(res.num);

                // mutex for collision safety and then free hashtable mutex:)
                printf("here\n");
                if (!shared_req->resolved) {
                    if ((write(shared_req->sd, res.bytes, sizeof(uint64_t))) == -1) {
                        perror("[server][!] write() failed");
                        exit(-1);
                    }
                    printf("[t:%d] found it.\n",id);

                    close(shared_req->sd);
                    
                    // mutex here or from the outer one
                    htable_set(ht, guess_hash, res.num);

                    shared_req->resolved = true;
                }
            }

            if (++num == range_end)
                num = range_start;
        }
    }

}

void init_workers(tpool_t *tp, uint8_t buffer[PACKET_REQUEST_SIZE], int nthreads)
{
    unsigned long range = get_range(buffer);
    unsigned int chunk = 1 + ((range - 1) / nthreads); // ceil(range/nthreads)-ish --- NOTE THE -ISH

    for (int i = 0; i < nthreads - 1; ++i) {
        init_t *setup      = malloc(sizeof(init_t));
        setup->id          = i;
        setup->range_start = i * chunk ;
        setup->range_end   = (i + 1) * chunk;

        tpool_add_work(tp, worker, setup);
    }
    init_t *setup      = malloc(sizeof(init_t));
    setup->id          = nthreads - 1;
    setup->range_start = (nthreads - 1) * chunk;
    setup->range_end   = range; 

    tpool_add_work(tp, worker, setup);
}

void launch_server_II(struct Server *server, int nthreads)
{
    int conn_sd, socklen;
    struct sockaddr_in cli_addr;
    socklen = sizeof(cli_addr);
    uint8_t buffer[PACKET_REQUEST_SIZE];
    tpool_t *tp = tpool_create(nthreads);
    ht = create_htable();
    //request_t *req_board[nthreads];
    // TODO: check attributes
    pthread_barrier_init(&barrier_0, NULL, nthreads);
    pthread_barrier_init(&barrier_1, NULL, nthreads);

    shared_req = malloc(sizeof(request_t));


    // 1. request for determining range
    if ((conn_sd = accept(server->socket, (struct sockaddr *)&cli_addr, &socklen)) < 0) {
        perror("[server][!] accept() failed");
        close(conn_sd);
        exit(-1);
    }

    bzero(buffer, PACKET_REQUEST_SIZE);
    if ((read(conn_sd, buffer, PACKET_REQUEST_SIZE)) == -1) {
        perror("[server][!] read() failed");
        exit(-1);
    }

    init_workers(tp, buffer, nthreads);

    submit_req(conn_sd, buffer);
    printf("req available\n");

    // >1 requests
    for (;;) {
        // TODO: Polling might still be viable
        if ((conn_sd = accept(server->socket, (struct sockaddr *)&cli_addr, &socklen)) < 0) {
            perror("[server][!] accept() failed");
            close(conn_sd);
        }

        bzero(buffer, PACKET_REQUEST_SIZE);
        if ((read(conn_sd, buffer, PACKET_REQUEST_SIZE)) == -1) {
            perror("[server][!] read() failed");
            exit(-1);
        }


        // if (req.prio >= 2) {
        //     enter panic mode
        // }

        submit_req(conn_sd, buffer);
    }
}