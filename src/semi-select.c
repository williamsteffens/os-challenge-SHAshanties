#define DEBUG 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <netinet/in.h>

#include <inttypes.h>

#include "os-challenge-util.h"





uint8_t buffer[PACKET_REQUEST_SIZE];
pid_t pid;

hashTable_t* hashtable;



int setupServer(short port, int backlog);
int acceptNewConnection(int servSocket);
int hashMatches(uint8_t hash[SHA256_DIGEST_LENGTH], uint8_t guessHash[SHA256_DIGEST_LENGTH]);
void SHASolver(int cliSocket);



int main(int argc, char *argv[]) {
   
   
   fd_set currentSockets, readySockets;
   FD_ZERO(&currentSockets);
   FD_SET(servSocket, &currentSockets);
   int maxSocketSoFar = servSocket; 

   hashtable = htInit();

   while (1) {
      readySockets = currentSockets; 
      
      // TODO: max + 1 ??? 
      if (select(maxSocketSoFar + 1, &readySockets, NULL, NULL, NULL) < 0) {
         perror("[server] error select()");
         exit(EXIT_FAILURE);
      }

      for (int i = 0; i <= maxSocketSoFar + 1; ++i)
         if (FD_ISSET(i, &readySockets))
            if (i == servSocket) {
               // There's data to read
               int cliSocket = acceptNewConnection(servSocket);
               FD_SET(cliSocket, &currentSockets);
               if (cliSocket > maxSocketSoFar)
                  maxSocketSoFar = cliSocket;
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

   // respond before anything else if we have the hash in the cache
   if (htContainsKey(hashtable, req.hash)) {
      uint64_t ans = htGet(hashtable, req.hash);
      if ((write(cliSocket, &ans, sizeof(uint64_t))) == -1) {
         perror("[server] error write()");
         exit(EXIT_FAILURE);
      }
      printf("\n\thashtabled, biatch\n");
      return;
   }

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
         #if DEBUG
            printf("ans: %" PRIu64 "\n\n", res.num);
         #endif
         res.num = htobe64(res.num);
         htSet(hashtable, req.hash, res.num);
         break;
      }
   }

   // Write response to client
   if ((write(cliSocket, res.bytes, sizeof(uint64_t))) == -1) {
      perror("[server] error write()");
      exit(EXIT_FAILURE);
   }

}

