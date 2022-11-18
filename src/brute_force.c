#include "brute_force.h"
#include "os-challenge-util.h"
#include <stdlib.h>
#include <string.h>

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
#include <pthread.h>



void brute_force_SHA(int conn_sd) 
{
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
    uint8_t guess_hash[SHA256_DIGEST_LENGTH];
    response_t res = {0};
    for (res.num = req.start; res.num < req.end; ++res.num) {
        SHA256(res.bytes, sizeof(uint64_t), guess_hash);
        if (memcmp(req.hash, guess_hash, SHA256_DIGEST_LENGTH) == 0) {
            res.num = htobe64(res.num);
            break;
        }
    }

    // Write response to client
    if ((write(conn_sd, res.bytes, sizeof(uint64_t))) == -1) {
        perror("[server] write() failed");
        exit(-1);
    }

    // Close the connection when done
    close(conn_sd);
}