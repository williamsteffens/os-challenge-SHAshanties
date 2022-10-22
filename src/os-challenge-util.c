#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "os-challenge-util.h"



request_t decode_req(int req_socket, uint8_t buffer[PACKET_REQUEST_SIZE])
{
    request_t req = {0};
    req.sd = req_socket;
    memcpy(&req.hash, buffer + PACKET_REQUEST_HASH_OFFSET, SHA256_DIGEST_LENGTH);
    memcpy(&req.start, buffer + PACKET_REQUEST_START_OFFSET, 8);
    memcpy(&req.end, buffer + PACKET_REQUEST_END_OFFSET, 8);
    req.prio = buffer[PACKET_REQUEST_PRIO_OFFSET];
    req.start = be64toh(req.start);
    req.end = be64toh(req.end);

    return req;
}

request_t *decode_preq(int req_socket, uint8_t buffer[PACKET_REQUEST_SIZE])
{
    request_t *preq = malloc(sizeof(request_t));
    
    preq->sd = req_socket;
    memcpy(&preq->hash, buffer + PACKET_REQUEST_HASH_OFFSET, SHA256_DIGEST_LENGTH);
    memcpy(&preq->start, buffer + PACKET_REQUEST_START_OFFSET, 8);
    memcpy(&preq->end, buffer + PACKET_REQUEST_END_OFFSET, 8);
    preq->start = be64toh(preq->start);
    preq->end = be64toh(preq->end);
    preq->prio = buffer[PACKET_REQUEST_PRIO_OFFSET];
    preq->resolved = false; 

    return preq;    
}

void display_request(uint8_t buffer[PACKET_REQUEST_SIZE], request_t req)
{
    memcpy(&req.hash, buffer + PACKET_REQUEST_HASH_OFFSET, SHA256_DIGEST_LENGTH);
    memcpy(&req.start, buffer + PACKET_REQUEST_START_OFFSET, 8);
    memcpy(&req.end, buffer + PACKET_REQUEST_END_OFFSET, 8);
    req.prio = buffer[PACKET_REQUEST_PRIO_OFFSET];

    req.start = be64toh(req.start);
    req.end = be64toh(req.end);

    printf("\thash: ");
    for (int i = SHA256_DIGEST_LENGTH - 1; i >= 0; --i)
        printf("%02x", req.hash[i]);
    printf("\n");

    printf("\tstart: %d\n", (int)req.start);
    printf("\tend:   %d\n", (int)req.end);
    printf("\tp:     %u\n", req.prio);
} 

// void err_check(int err_code, int todo, int compared_to_this, int sock_to_close)
// {
//    perror();
//    if (sock_to_close != -1) {
//       close(sock_to_close);
//    }
//    exit(EXIT_FAILURE);
// }