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
int listen_sd; 
int on = 1;
int conn_sd, clilen;
uint8_t buffer[PACKET_REQUEST_SIZE];
struct pollfd fds[200];
struct sockaddr_in serv_addr, cli_addr;
pid_t pid;
int timeout;
int poll_status; 

uint64_t answer;



int bruteForceHash(uint8_t hash[SHA256_DIGEST_LENGTH], uint8_t guessHash[SHA256_DIGEST_LENGTH]);
void SHASolver(int conn_sd);



int main(int argc, char *argv[]) {
   if (argc != 2) {
      fprintf(stderr, "Incorrect usage. Usage: %listen_sd port\n", argv[0]);
      exit(-1);
   }   

   // First Argument should be the socket
   port = (unsigned short) atoi(argv[1]);
   
   // Get socket for accepting connections
   if ((listen_sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("[server] socket() failed");
      close(listen_sd);
      exit(-1);
   }

   // Set socket option to reuse the addr/port without waiting for it to be release 
   if ((setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0) {
      perror("[server] setsockopt() failed");
      close(listen_sd);
      exit(-1);
   }

   // Set socket to be non-blocking. All clientsockets will inherit this from the listening socket.
   if ((ioctl(listen_sd, FIONBIO, &on)) < 0) {
      perror("[server] ioctl() failed");
      close(listen_sd);
      exit(-1);
   }

   // Bind socket to server addr
   bzero((char *) &serv_addr, sizeof(serv_addr));
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(port);
   
   if ((bind(listen_sd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) < 0) {
      perror("[server] bind() failed");
      close(listen_sd);
      exit(-1);
   }
   
   // Listen for connection, backlog = 5
   if ((listen(listen_sd,5)) != 0) {
      perror("[server] listen() failed");
      close(listen_sd);
      exit(-1);
   }
   printf("[server] Listening on port %d...\n", port);
   
   // Initialize the pollfd struct 
   memset(fds, 0 , sizeof(fds));

   // Set up the initial listening socket
   fds[0].fd = listen_sd;
   fds[0].events = POLLIN;

   // Set timeout to 3 minutes; if no activity for 3 min, the program will terminate
   timeout = (3 * 60 * 1000);

   // Accept connection from client
   // clilen = sizeof(cli_addr);

   while (1) {
      // Wait for poll and wait 3 minuntes for it to complete
      printf("Waiting on poll()...\n");
      poll_status = poll(fds, nfds, timeout)
      if (poll_status < 0) {
         perror("[server] poll() failed");
         close(listen_sd);
         exit(-1);
      }
      else if (poll_status == 0) {
         printf("[server] poll() timeout. Terminating server... \n");
         close(listen_sd);
         break; 
      }

      // One or more descriptors are readable. Iterate and find out which ones
      







      if ((conn_sd = accept(listen_sd, (struct sockaddr *)&cli_addr, &clilen)) == -1) {
         perror("[server] accept() failed");
         close(listen_sd);
         exit(-1);
      }

      if ((pid = fork()) < 0)  {
         perror("[server] fork() failed");
         close(listen_sd);
         exit(-1);
      } 

      if (pid == 0) {
         close(listen_sd);
         SHASolver(conn_sd);
         exit(-1);
      } else {
         close(conn_sd);
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

void SHASolver(int conn_sd) {
   // Read request from client
   bzero(buffer, PACKET_REQUEST_SIZE);
   if ((read(conn_sd, buffer, PACKET_REQUEST_SIZE)) == -1) {
      perror("[server] read() failed");
   exit(-1);
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
   if ((write(conn_sd, res.bytes, sizeof(uint64_t))) == -1) {
      perror("[server] write() failed");
      exit(-1);
   }
}