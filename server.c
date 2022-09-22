#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include <openssl/sha.h>
#include "messages.h"

/* Data structure to easily shift between 8 byte arrays
 * and 64-bit unsigned integers */
union Bound {
   uint8_t raw[PACKET_RESPONSE_SIZE];
   uint64_t v;
};


/* This function does all the actual processing */
void process_packet(int sock) {
   int n;
   uint8_t buffer[PACKET_REQUEST_SIZE];
   uint8_t hash[SHA256_DIGEST_LENGTH];
   union Bound start, end;
   uint8_t priority;

   /* If connection is established then start communicating */
   bzero(buffer,PACKET_REQUEST_SIZE);
   n = read( sock,buffer,PACKET_REQUEST_SIZE );

   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }

   /* Extract start and end values from the packet */
   bzero(start.raw, PACKET_RESPONSE_SIZE);
   bzero(end.raw, PACKET_RESPONSE_SIZE);

   memcpy(start.raw, buffer+PACKET_REQUEST_START_OFFSET, PACKET_RESPONSE_SIZE);
   memcpy(end.raw, buffer+PACKET_REQUEST_END_OFFSET, PACKET_RESPONSE_SIZE);

   /* Convert to appropriate byte order */
   start.v = be64toh(start.v);
   end.v = be64toh(end.v);

   /* Extract priority from the packet (currently unused) */
   priority = buffer[PACKET_REQUEST_PRIO_OFFSET];
   
   /* Load hashing value from packet into our array */
   bzero(hash, SHA256_DIGEST_LENGTH);
   memcpy(hash, buffer, SHA256_DIGEST_LENGTH);

   /* variables to use for brute force testing */
   uint64_t i;
   union Bound no_to_hash, result;
   uint8_t hash_found[SHA256_DIGEST_LENGTH];
   int flag_match = 0;

   /* This for-loop brute force checks the hashing value
    * of numbers from start to end */
   for (i = start.v; i < end.v; i++) {
      no_to_hash.v = i;

      /* Hash the current number */
      SHA256(no_to_hash.raw, PACKET_RESPONSE_SIZE, hash_found);

      /* Check if there's a match (and don't waste
       * time on mismatches) */
      for (int j = 0; j < SHA256_DIGEST_LENGTH; j++) {
         if (hash[j] == hash_found[j]) {
            if (j == SHA256_DIGEST_LENGTH-1) {
               flag_match = 1;
               break;
            }
         } else {
            break;
         }
      }
      if (flag_match) {
         result.v = i;
         break;
      }
   }

   /* Convert the result to the appriate byte order */
   result.v = htobe64(result.v);

   /* Write a response to the client */
   n = write(sock,result.raw,PACKET_RESPONSE_SIZE);

   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }

}


int main( int argc, char *argv[] ) {
   int sockfd, newsockfd, portno, clilen;
   uint8_t buffer[PACKET_REQUEST_SIZE];
   struct sockaddr_in serv_addr, cli_addr;
   int  n, pid;

   /* Use command-line input */
   if (argc == 2) {
       portno = atoi(argv[1]);
       printf("Connecting to port no: %i\n", portno);
   }
   else {
       printf("Error: Expected one argument (port number)");
       exit(1);
   }

   /* First call to socket() function */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);

   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }

   /* Initialize socket structure */
   bzero((char *) &serv_addr, sizeof(serv_addr));


   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);

   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }

   /* Now start listening for the clients, here process will
      * go in sleep mode and will wait for the incoming connection
   */

   listen(sockfd,PACKET_REQUEST_SIZE);
   clilen = sizeof(cli_addr);

   while (1) {
      /* Accept actual connection from the client */
      newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

      if (newsockfd < 0) {
         perror("ERROR on accept");
         exit(1);
      }

      /* Create a child process for each packet */
      pid = fork();

      if (pid < 0) {
         perror("ERROR on fork");
         exit(1);
      }

      if (pid == 0) {
         close(sockfd);
         process_packet(newsockfd);
         exit(0);
      }
      else {
         close(newsockfd);
      }
   }

   return 0;
}
