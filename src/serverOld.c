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



typedef union request {
   uint8_t hash[SHA256_DIGEST_LENGTH];
   uint64_t start;
   uint64_t end;
   uint8_t prio;
} request;



void doprocessing(int sock);
void setup_server();



int main(int argc, char *argv[]) {
   
   if (argc != 2) {
      fprintf(stderr, "Incorrect usage. Usage: %s port\n", argv[0]);
      exit(1);
   }   

   int port = atoi(argv[1]);
   
   int newsockfd, clilen;
   uint8_t buffer[PACKET_REQUEST_SIZE];
   struct sockaddr_in serv_addr, cli_addr;
   int n, pid;
   
   int sockfd = socket(AF_INET, SOCK_STREAM, 0);
   int option = 1;
   setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));  

   if (sockfd < 0) {
      perror("[server] ERROR opening socket");
      exit(1);
   }
      
   bzero((char *) &serv_addr, sizeof(serv_addr));
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(port);
   
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("[server] ERROR on binding");
      exit(1);
   }
   
   listen(sockfd,5);
   clilen = sizeof(cli_addr);

   printf("[server] Listening on port %d...\n", port);
   
   /* Accept actual connection from the client */
   newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
	
   if (newsockfd < 0) {
      perror("[server] ERROR on accept");
      exit(1);
   }
   
   /* If connection is established then start communicating */
   bzero(buffer, PACKET_REQUEST_SIZE);

   n = read(newsockfd, buffer, PACKET_REQUEST_SIZE);

   if (n < 0) {
      perror("[server] ERROR reading from socket");
      exit(1);
   }


   // request* req;

   uint8_t hash[SHA256_DIGEST_LENGTH];
   uint64_t start;
   uint64_t end;
   uint8_t prio;

   bzero(&hash, SHA256_DIGEST_LENGTH);
   bzero(&start, 8);
   bzero(&end, 8);
   bzero(&prio, 1);

   memcpy(&hash, buffer + PACKET_REQUEST_HASH_OFFSET, SHA256_DIGEST_LENGTH);
   memcpy(&start, buffer + PACKET_REQUEST_START_OFFSET, 8);
   memcpy(&end, buffer + PACKET_REQUEST_END_OFFSET, 8);
   prio = buffer[PACKET_REQUEST_PRIO_OFFSET];

   // uint8_t rehash[SHA256_DIGEST_LENGTH]; 
   
   // for (int i = SHA256_DIGEST_LENGTH; i >= 0; --i) {
   //    rehash[SHA256_DIGEST_LENGTH - i] = hash[i];
   // }

   // for (int i = 0; i < 32; ++i) {
   //    printf("%02x", rehash[i]);
   // }
   // printf("\n");

   
   printf("start: %lu\n", be64toh(start));
   printf("end:   %lu\n", be64toh(end));
   printf("p:     %u\n", prio);


   

   // printf("%hhn\n", req.hash);
   // printf("%lu\n", startAndEnd[0]);
   // printf("%lu\n", startAndEnd[1]);
   // printf("%d\n", buffer[48]);
   // printf("%d\n", req.prio);






   //uint64_t res = 0x6b86b273ff34fce19d6b804eff5a3f5747ada4eaa22f1d49c01e52ddb7875b4b;


   //printf("%d", u.priority);

   // printf("%u\n", buffer[48]);

   // printf("%hhn", buffer);

   
   // char* cp = buffer;
   // for ( ; *cp != '\0'; ++cp )
   // {
   //    printf("%d", *cp);
   // }
   
   //printf("\n");

   //uint64_t er[] = {4};

   /* Write a response to the client */

   n = write(newsockfd, "A", 32);
   
   if (n < 0) {
      perror("[server] ERROR writing to socket");
      exit(1);
   }





   // while (1) {
   //    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		
   //    if (newsockfd < 0) {
   //       perror("ERROR on accept");
   //       exit(1);
   //    }
      
   //    pid = fork();
		
   //    if (pid < 0) {
   //       perror("ERROR on fork");
   //       exit(1);
   //    }
      
   //    if (pid == 0) {
   //       /* This is the client process */
   //       close(sockfd);
   //       doprocessing(newsockfd);
   //       exit(0);
   //    } else {
   //       close(newsockfd);
   //    }
   // }

   // ???
   // end()
   // read()
   
      
   return 0;
}



void doprocessing(int sock) {
   int n;
   char buffer[256];
   bzero(buffer,256);
   n = read(sock,buffer,255);
   
   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }
   
   printf("Here is the message: %s\n",buffer);
   n = write(sock,"I got your message",18);
   
   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }
	
}

void setup_server() {

}
