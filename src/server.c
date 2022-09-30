#define DEBUG 0

#include <stdio.h>
#include <stdlib.h>
// TODO: check for the need of the includes
#include <errno.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

// is included in messages.h
// #include <openssl/sha.h>

#include <string.h>

#include "SHASHArealsolved.h"
#include "messages.h"



typedef struct _request {
   uint8_t hash[SHA256_DIGEST_LENGTH];
   uint64_t start;
   uint64_t end;
   uint8_t prio;
} request;

typedef union _response {
   uint64_t num;
   uint8_t bytes[8];
} response; 


unsigned short port; 
int s; 
int ns, clilen;
uint8_t buffer[PACKET_REQUEST_SIZE];
struct sockaddr_in serv_addr, cli_addr;
int pid;

uint64_t answer;



int bruteForceHash(uint8_t hash[SHA256_DIGEST_LENGTH], uint8_t guessHash[SHA256_DIGEST_LENGTH]);
void SHASolver(int ns);



int main(int argc, char *argv[]) {
   if (argc != 2) {
      fprintf(stderr, "Incorrect usage. Usage: %s port\n", argv[0]);
      exit(1);
   }   

   // First Argument should be the socket
   port = (unsigned short) atoi(argv[1]);
   
   // Get socket for accepting connections
   if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("[server] error socket()");
      exit(2);
   }

   // Set socket option to reuse the addr/port without waiting for it to be release 
   int optval = 1;
   if ((setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))) < 0) {
      perror("[server] error setsockopt()");
      exit(3);
   }

   // Bind socket to server addr
   bzero((char *) &serv_addr, sizeof(serv_addr));
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(port);
   
   if ((bind(s, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) < 0) {
      perror("[server] error bind()");
      exit(4);
   }
   
   // Listen for connection, backlog = 5
   if ((listen(s,5)) != 0) {
      perror("[server] error listen()");
      exit(5);
   }
   printf("[server] Listening on port %d...\n", port);
   
   // Accept connection from client
   clilen = sizeof(cli_addr);

   while (1) {
      if ((ns = accept(s, (struct sockaddr *)&cli_addr, &clilen)) == -1) {
         perror("[server] error accept()");
         exit(6);
      }

      if ((pid = fork()) < 0)  {
         perror("[server] error fork()");
         exit(6);
      } 

      if (pid == 0) {
         close(s);
         SHASolver(ns);
         exit(0);
      } else {
         close(ns);
      } 
   }
	
   printf("[server] Server ended successfully\n");
      
   return 0;
}

int bruteForceHash(uint8_t hash[SHA256_DIGEST_LENGTH], uint8_t guessHash[SHA256_DIGEST_LENGTH]) {
   for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
      if (hash[i] != guessHash[i])
         return 0;
   
   return 1;
}

void SHASolver(int ns) {
   // Read request from client
   bzero(buffer, PACKET_REQUEST_SIZE);
   if ((read(ns, buffer, PACKET_REQUEST_SIZE)) == -1) {
      perror("[server] error read()");
      exit(7);
   }

   // Zerolize vars
   request req = {0};

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
   uint8_t guessHash[SHA256_DIGEST_LENGTH];
   response res = {0};
   for (res.num = req.start; res.num < req.end; ++res.num) {
      SHA256(res.bytes, sizeof(uint64_t), guessHash);
      if (bruteForceHash(req.hash, guessHash)) {
         res.num = htobe64(res.num);
         break;
      }
   }

   // Write response to client
   if ((write(ns, res.bytes, sizeof(uint64_t))) == -1) {
      perror("[server] error write()");
      exit(8);
   }
}