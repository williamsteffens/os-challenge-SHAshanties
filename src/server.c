#define DEBUG 0

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <netinet/in.h>
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

uint8_t buffer[PACKET_REQUEST_SIZE];
pid_t pid;

uint64_t answer;



int setupServer(short port, int backlog);
int acceptNewConnection(int servSocket);
int hashMatches(uint8_t hash[SHA256_DIGEST_LENGTH], uint8_t guessHash[SHA256_DIGEST_LENGTH]);
void SHASolver(int cliSocket);



int main(int argc, char *argv[]) {
   if (argc != 2) {
      fprintf(stderr, "Incorrect usage. Usage: %s port\n", argv[0]);
      exit(EXIT_FAILURE);
   }   

   // First Argument should be the socket
   short port = (short) atoi(argv[1]);
   
   // TODO: Look into the backlog stuff
   int servSocket = setupServer(port, 100);

   fd_set currentSockets, readySockets;
   FD_ZERO(&currentSockets);
   FD_SET(servSocket, &currentSockets);

   while (1) {
      readySockets = currentSockets; 

      if (select(FD_SETSIZE, &readySockets, NULL, NULL, NULL) < 0) {
         perror("[server] error select()");
         exit(EXIT_FAILURE);
      }

      for (int i = 0; i < FD_SETSIZE; ++i)
         if (FD_ISSET(i, &readySockets))
            if (i == servSocket) {
               // There's data to read
               int cliSocket = acceptNewConnection(servSocket);
               FD_SET(cliSocket, &currentSockets);
            } 
            else {
               SHASolver(i);
               FD_CLR(i, &currentSockets);
            }

      // Accept connection from client

      // if ((pid = fork()) < 0)  {
      //    perror("[server] error fork()");
      //    exit(EXIT_FAILURE);
      // } 

      // if (pid == 0) {
      //    close(servSocket);
      //    SHASolver(cliSocket);
      //    exit(0);
      // } else {
      //    close(cliSocket);
      // } 
   }
	
   printf("[server] Server ended successfully\n");
      
   return EXIT_SUCCESS;
}



int setupServer(short port, int backlog) {
   int servSocket;
   struct sockaddr_in serv_addr;

   // Get socket for accepting connections
   if ((servSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("[server] error socket()");
      exit(EXIT_FAILURE);
   }

   // Set socket option to reuse the addr/port without waiting for it to be release 
   int optVal = 1;
   if ((setsockopt(servSocket, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal))) < 0) {
      perror("[server] error setsockopt()");
      exit(EXIT_FAILURE);
   }

   // Bind socket to server addr
   bzero((char *) &serv_addr, sizeof(serv_addr));
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(port);
   
   if ((bind(servSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) < 0) {
      perror("[server] error bind()");
      exit(EXIT_FAILURE);
   }
   
   // Listen for connection, backlog = 5
   if ((listen(servSocket, backlog)) != 0) {
      perror("[server] error listen()");
      exit(EXIT_FAILURE);
   }
   printf("[server] Listening on port %d...\n", port);

   return servSocket;
}

int acceptNewConnection(int servSocket) {
   int cliSocket;
   struct sockaddr_in cli_addr;
   int clilen = sizeof(cli_addr);

   if ((cliSocket = accept(servSocket, (struct sockaddr *)&cli_addr, &clilen)) == -1) {
      perror("[server] error accept()");
      exit(EXIT_FAILURE);
   }

   return cliSocket;
}

int hashMatches(uint8_t hash[SHA256_DIGEST_LENGTH], uint8_t guessHash[SHA256_DIGEST_LENGTH]) {
   for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
      if (hash[i] != guessHash[i])
         return 0;
   
   return 1;
}

void SHASolver(int cliSocket) {
   // Read request from client
   bzero(buffer, PACKET_REQUEST_SIZE);
   if ((read(cliSocket, buffer, PACKET_REQUEST_SIZE)) == -1) {
      perror("[server] error read()");
      exit(EXIT_FAILURE);
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
      if (hashMatches(req.hash, guessHash)) {
         res.num = htobe64(res.num);
         break;
      }
   }

   // Write response to client
   if ((write(cliSocket, res.bytes, sizeof(uint64_t))) == -1) {
      perror("[server] error write()");
      exit(EXIT_FAILURE);
   }
}
