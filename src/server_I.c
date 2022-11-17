// /*
//     Description: 
//         + thread pool of 4
//         + 4 threads 1 client
//         + hashtable cache

//         - no prio functionality
//         - no epoll I/O multiplexing


// */

// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>
// #include <stdbool.h>
// #include <sys/socket.h>
// #include <netdb.h>
// #include <netinet/in.h>
// #include <unistd.h>
// #include <errno.h>

// #include <sys/types.h>
// #include "server.h"
// #include "server_I.h"
// #include "os-challenge-util.h"
// #include "thread_pool.h"
// #include "threaded_server.h"
// #include "hash_table.h"



// void brute_force_worker_I(void *arg)
// {
//     bf_task_t *ptask = (bf_task_t*)arg; 
//     uint8_t guess_hash[SHA256_DIGEST_LENGTH];
//     response_t res = {0};

//     for (res.num = ptask->start; res.num < ptask->end; ++res.num) {
//         if (*(ptask->done) == true) 
//             break;

//         SHA256(res.bytes, sizeof(uint64_t), guess_hash);
//         if (memcmp(ptask->hash, guess_hash, SHA256_DIGEST_LENGTH) == 0) {
//             res.num = htobe64(res.num);
//             if ((write(ptask->sd, res.bytes, sizeof(uint64_t))) == -1) {
//                 perror("[server][!] write() failed");
//                 exit(-1);
//             }

//             close(ptask->sd);

//             htable_set(ptask->ht, guess_hash, res.num);

//             // tell tpool to halt this round, it will then continue to the next
//             *(ptask->done) = true;
//             free(ptask->done);

//             break;
//         }
//     }

//     free(ptask);
// }

// void assign_work_I(tpool_t *tp, htable_t *ht, request_t req, int ntasks)
// {
//     unsigned long range = req.end - req.start;
//     unsigned int chunk = 1 + ((range - 1) / ntasks); // ceil(range/nthreads)-ish --- NOTE THE -ISH
    
//     bool *done = malloc(sizeof(bool));
//     *done = false;
//     for (int i = 0; i < ntasks - 1; ++i) {
//         bf_task_t *ptask = malloc(sizeof(bf_task_t));
//         ptask->done  = done;
//         memcpy(&ptask->hash, &req.hash, SHA256_DIGEST_LENGTH);
//         ptask->sd    = req.sd;
//         ptask->start = req.start + i * chunk;
//         ptask->end   = req.start + (i + 1) * chunk;
//         ptask->ht    = ht;

//         tpool_add_work(tp, brute_force_worker_I, ptask);
//     }
//     bf_task_t *ptask = malloc(sizeof(bf_task_t));
//     ptask->done  = done;
//     memcpy(&ptask->hash, &req.hash, SHA256_DIGEST_LENGTH);
//     ptask->sd    = req.sd;
//     ptask->start = req.start + (ntasks - 1) * chunk;
//     ptask->end   = req.end;
//     ptask->ht    = ht;

//     tpool_add_work(tp, brute_force_worker_I, ptask);

// }

// void coordinator_I(int listen_socket, int nthreads, int ntasks)
// {
//     int conn_sd, socklen;
//     struct sockaddr_in cli_addr;
//     socklen = sizeof(cli_addr);
//     uint8_t buffer[PACKET_REQUEST_SIZE];
//     tpool_t *tp = tpool_create(nthreads);
//     htable_t *ht = create_htable();

//     for (;;) {
//         // TODO: Polling might still be viable
//         if ((conn_sd = accept(listen_socket, (struct sockaddr *)&cli_addr, &socklen)) < 0) {
//             perror("[server][!] accept() failed");
//             close(conn_sd);
//             exit(-1);
//         }

//         bzero(buffer, PACKET_REQUEST_SIZE);
//         if ((read(conn_sd, buffer, PACKET_REQUEST_SIZE)) == -1) {
//             perror("[server][!] read() failed");
//             exit(-1);
//         }
        
//         // TODO: could split it up, so we return the hashed request after reading the hash and dont bother with the additional info
//         request_t req = decode_req(conn_sd, buffer);
        
//         if (htable_contains_key(ht, req.hash)) {
//             uint64_t ans = htable_get(ht, req.hash);
//             if ((write(conn_sd, &ans, sizeof(uint64_t))) == -1) {
//                 perror("[server][!] write() failed");
//                 exit(-1);
//             }

//             // DEBUG
//             // printf("hashed cached hash from hashtable:\n\t");
//             // printf("%lu\n", be64toh(ans));
//             continue; 
//         }
//             // if isinhashtable
//                 // send
//             // if current_processing
//                 // wait and send -- or -- make a thread that will handle it once it gets woken by the workers 
        
//         // wait should be here so that the prio of the next request and be assessed
//         // tpool_wait(tp);

//         assign_work_I(tp, ht, req, ntasks);

//         tpool_wait(tp);
//     }

//     tpool_destroy(tp);

// }

// void launch_server_I(struct Server *server, int nthreads, int ntasks)
// {
//     coordinator_I(server->socket, nthreads, ntasks);
// }