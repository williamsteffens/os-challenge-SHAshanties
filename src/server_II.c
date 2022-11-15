#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>

#include <sched.h>

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
pthread_barrier_t barrier_panic;
pthread_mutex_t queue_mutex_II = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t send_and_hash_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond_var_II = PTHREAD_COND_INITIALIZER;
pthread_cond_t hash_cond_var = PTHREAD_COND_INITIALIZER;
htable_t *ht;

bool panic_mode;
bool skipped_last; 

request_t *shared_req;
request_t *panic_req;



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

void submit_req(request_t *preq)
{
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
    response_t  res = {0};
    // TODO: do a lap and if it was not resolved wait?
    unsigned long entered_at = 0; 


    unsigned long num = range_start; 
    for (;;) {
        pthread_barrier_wait(&barrier_0);
            
        if (id == 0) {
            if (shared_req != NULL) {
                free(shared_req);
            }
            
            pthread_mutex_lock(&queue_mutex_II);
                while ((shared_req = dequeue_req()) == NULL)
                    pthread_cond_wait(&queue_cond_var_II, &queue_mutex_II);
            pthread_mutex_unlock(&queue_mutex_II);
        }

        pthread_barrier_wait(&barrier_1);

        if (num == range_start)
            entered_at = range_end;
        else 
            entered_at = num - 1;
        

        while (!shared_req->resolved && num != entered_at) {
            if (panic_mode) {
                unsigned long panic_num = num;
                unsigned long panic_entered_at;
                if (panic_num == range_start)
                    panic_entered_at = range_end;
                else 
                    panic_entered_at = panic_num - 1;


                while (panic_mode && panic_num != panic_entered_at) {
                    res.num = panic_req->start + panic_num;

                    SHA256(res.bytes, sizeof(uint64_t), guess_hash);
                    if (panic_req->hash[0] == guess_hash[0]) {
                        if (memcmp(panic_req->hash, guess_hash, SHA256_DIGEST_LENGTH) == 0) {
                            res.num = htobe64(res.num);

                            // mutex for collision safety and then free hashtable mutex
                            pthread_mutex_lock(&send_and_hash_mutex);
                                if (!panic_req->resolved) {
                                    if ((write(panic_req->sd, res.bytes, sizeof(uint64_t))) == -1) {
                                        perror("[server][!] write() failed");
                                        exit(-1);
                                    }

                                    close(panic_req->sd);

                                    // semaphore here?                         
                                    htable_set(ht, guess_hash, res.num);
                                    pthread_cond_signal(&hash_cond_var);

                                    panic_mode = false;
                                }
                            pthread_mutex_unlock(&send_and_hash_mutex);
                        }
                    }

                    if (++panic_num == range_end)
                        panic_num = range_start;
                }

                //printf("[server][t:%d] I'm waiting in panic.\n", id);
                pthread_barrier_wait(&barrier_panic);
            }
        
            res.num = shared_req->start + num;

            SHA256(res.bytes, sizeof(uint64_t), guess_hash);
            if (shared_req->hash[0] == guess_hash[0]) {
                if (memcmp(shared_req->hash, guess_hash, SHA256_DIGEST_LENGTH) == 0) {
                    res.num = htobe64(res.num);

                    // mutex for collision safety and then free hashtable mutex
                    pthread_mutex_lock(&send_and_hash_mutex);
                        if (!shared_req->resolved) {
                            if ((write(shared_req->sd, res.bytes, sizeof(uint64_t))) == -1) {
                                perror("[server][!] write() failed");
                                exit(-1);
                            }

                            close(shared_req->sd);

                            // semaphore here?                         
                            htable_set(ht, guess_hash, res.num);
                            pthread_cond_signal(&hash_cond_var);


                            shared_req->resolved = true;
                        }
                    pthread_mutex_unlock(&send_and_hash_mutex);
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
    pthread_barrier_init(&barrier_panic, NULL, nthreads);

    request_t *preq;
    shared_req = malloc(sizeof(request_t));
    shared_req = NULL;
    panic_req = malloc(sizeof(request_t));


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
    
    
    
    // request_t req = decode_req(conn_sd, buffer); 
    // display_request(buffer, req);
    
    
    

    preq = decode_preq(conn_sd, buffer);

    submit_req(preq);

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

        // request_t req = decode_req(conn_sd, buffer); 
        // display_request(buffer, req);

        // TODO: could split it up, so we return the hashed request after reading the hash and dont bother with the additional info
        preq = decode_preq(conn_sd, buffer);


        // TODO: do we need a mutex here for the hashtable? 
        if (htable_contains_key(ht, preq->hash)) {
            pthread_mutex_lock(&send_and_hash_mutex);
            while (htable_get(ht, preq->hash) == 0)
                pthread_cond_wait(&hash_cond_var, &send_and_hash_mutex);
            pthread_mutex_unlock(&send_and_hash_mutex);

            uint64_t ans = htable_get(ht, preq->hash);
            if ((write(conn_sd, &ans, sizeof(uint64_t))) == -1) {
                perror("[server][!] write() failed");
                exit(-1);
            }
            
            continue; 
        } 
        else {
            if (preq->prio > 2 && !skipped_last) {
                panic_req = preq;
                panic_mode = true;
                skipped_last = true; 
            }
            else {
                htable_set(ht, preq->hash, 0);
                skipped_last = false; 
                submit_req(preq);
            }
        }

        // if (req.prio >= 2) {
        //     enter panic mode
        // }
    }
}