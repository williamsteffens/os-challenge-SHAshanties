#include <stdio.h>
#include <string.h>
#include "os-challenge-util.h"

int hashMatches(uint8_t hash[SHA256_DIGEST_LENGTH], uint8_t guessHash[SHA256_DIGEST_LENGTH])
{
   if (memcmp(hash, guessHash, SHA256_DIGEST_LENGTH) == 0)
      return 1;
   else 
      return 0;
}

void display_request(char buffer[PACKET_REQUEST_SIZE], request_t req)
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