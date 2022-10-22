#pragma once

#if defined(__linux__)
  #include <endian.h>
#elif defined(__APPLE__)
  #include <libkern/OSByteOrder.h>
  #define htobe64(x) OSSwapHostToBigInt64(x)
  #define htole64(x) OSSwapHostToLittleInt64(x)
  #define be64toh(x) OSSwapBigToHostInt64(x)
  #define le64toh(x) OSSwapLittleToHostInt64(x)
#endif

#include <openssl/sha.h>
#include <stdbool.h>



#define PACKET_REQUEST_SIZE           (SHA256_DIGEST_LENGTH + 8 + 8 + 1)
#define PACKET_REQUEST_HASH_OFFSET    0
#define PACKET_REQUEST_START_OFFSET   (SHA256_DIGEST_LENGTH)
#define PACKET_REQUEST_END_OFFSET     (SHA256_DIGEST_LENGTH + 8)
#define PACKET_REQUEST_PRIO_OFFSET    (SHA256_DIGEST_LENGTH + 8 + 8)

#define PACKET_RESPONSE_SIZE          8
#define PACKET_RESPONSE_ANSWER_OFFSET 0



typedef struct request {
  int      sd;
  uint8_t  hash[SHA256_DIGEST_LENGTH];
  uint64_t start;
  uint64_t end;
  uint8_t  prio;
  bool     resolved;
} request_t;

typedef union response {
  uint64_t num;
  uint8_t  bytes[8];
} response_t; 



bool sha_cmp(uint8_t hash[SHA256_DIGEST_LENGTH], uint8_t guess_hash[SHA256_DIGEST_LENGTH]);
request_t decode_req(int req_socket, uint8_t buffer[PACKET_REQUEST_SIZE]);
request_t *decode_preq(int req_socket, uint8_t buffer[PACKET_REQUEST_SIZE]);
void display_request(uint8_t buffer[PACKET_REQUEST_SIZE], request_t req);