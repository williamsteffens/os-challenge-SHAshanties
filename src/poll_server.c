// #define DEBUG 0

// #include <stdio.h>
// #include <stdlib.h>
// #include <stdbool.h>
// // TODO: check for the need of the includes
// #include <errno.h>

// #include <unistd.h>

// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netdb.h>
// #include <netinet/in.h>

// #include <sys/ioctl.h>
// #include <sys/poll.h>
// #include <arpa/inet.h>

// // is included in messages.h
// // #include <openssl/sha.h>

// #include <string.h>

// #include "os-challenge-util.h"


// unsigned short port; 
// int listen_sd; 
// int on = 1;
// int conn_sd, clilen, rc, len;
// uint8_t buffer[PACKET_REQUEST_SIZE];
// struct pollfd fds[200];
// struct sockaddr_in serv_addr, cli_addr;
// // test this?
// struct addrinfo; 
// pid_t pid;
// int timeout;
// int poll_status; 
// int nfds = 1, current_size = 0; 
// bool close_conn, compress_array;
// int step; 

// uint64_t answer;



// int bruteForceHash(uint8_t hash[SHA256_DIGEST_LENGTH], uint8_t guessHash[SHA256_DIGEST_LENGTH]);
// void SHASolver(int conn_sd);



// int main(int argc, char *argv[]) {

//    // gay loop stuff
//    int i, j; 


//    if (argc != 2) {
//       fprintf(stderr, "Incorrect usage. Usage: %s port\n", argv[0]);
//       exit(-1);
//    }   

//    // First Argument should be the socket
//    port = (unsigned short) atoi(argv[1]);
   
//    // Get socket for accepting connections
//    if ((listen_sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
//       perror("[server] socket() failed");
//       close(listen_sd);
//       exit(-1);
//    }

//    // Set socket option to reuse the addr/port without waiting for it to be release 
//    if ((setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0) {
//       perror("[server] setsockopt() failed");
//       close(listen_sd);
//       exit(-1);
//    }

//    // Set socket to be non-blocking. All clientsockets will inherit this from the listening socket.
//    if ((ioctl(listen_sd, FIONBIO, &on)) < 0) {
//       perror("[server] ioctl() failed");
//       close(listen_sd);
//       exit(-1);
//    }

//    // Bind socket to server addr
//    bzero((char *) &serv_addr, sizeof(serv_addr));
   
//    serv_addr.sin_family = AF_INET;
//    serv_addr.sin_addr.s_addr = INADDR_ANY;
//    serv_addr.sin_port = htons(port);
   
//    if ((bind(listen_sd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) < 0) {
//       perror("[server] bind() failed");
//       close(listen_sd);
//       exit(-1);
//    }
   
//    // Listen for connection, backlog = 100
//    if ((listen(listen_sd,100)) != 0) {
//       perror("[server] listen() failed");
//       close(listen_sd);
//       exit(-1);
//    }
//    printf("[server] Listening on port %d...\n", port);
   
//    // Initialize the pollfd struct 
//    memset(fds, 0 , sizeof(fds));

//    // Set up the initial listening socket
//    fds[0].fd = listen_sd;
//    fds[0].events = POLLIN;

//    // Set timeout to 3 minutes; if no activity for 3 min, the program will terminate
//    timeout = (3 * 60 * 1000);

//    // Accept connection from client
//    // clilen = sizeof(cli_addr);

//    for(;;) {
//       // Wait for poll and wait 3 minuntes for it to complete
//       printf("[server] Waiting on poll()...\n");
//       poll_status = poll(fds, nfds, timeout);
//       if (poll_status < 0) {
//          perror("[server] poll() failed");
//          close(listen_sd);
//          exit(-1);
//       }
//       else if (poll_status == 0) {
//          printf("[server] Timeout reached. Terminating server... \n");
//          close(listen_sd);
//          break; 
//       }

//       // One or more descriptors are readable. Iterate and find out which ones
//       current_size = nfds; 
//       for (i = 0; i < current_size; i++) {
//          if (fds[i].revents == 0)
//             continue; 

//          if (fds[i].revents != POLLIN) {
//             perror("[server] revents different from POLLIN");
//             close(listen_sd);
//             exit(-1);
//          }

//          if (fds[i].fd == listen_sd) {
//             do {
//                if ((conn_sd = accept(listen_sd, (struct sockaddr *)&cli_addr, &clilen)) < 0) {
//                   if (errno != EWOULDBLOCK) {
//                      perror("[server] accept() failed");
//                      exit(EXIT_FAILURE); 
//                   }
//                   break;
//                }
               
//                char outAddress[69 + 1];

//                inet_ntop(AF_INET, (char *)&(cli_addr.sin_addr), buffer, sizeof(cli_addr));
// 				   printf("[+] connected with %s:%d\n", buffer, ntohs(cli_addr.sin_port));

//                printf("[server] New incoming connection - %d\n", conn_sd);
//                fds[nfds].fd = conn_sd;
//                fds[nfds].events = POLLIN;
//                nfds++;
//             } while (conn_sd != -1);
//          }
//          else {
//             printf("[server] \tDescriptor %d is readable\n", fds[i].fd);
//             close_conn = false;

//             do {
//                // rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
//                // if (rc < 0) {
//                //    if (errno != EWOULDBLOCK) {
//                //       perror("[server] \trecv() failed");
//                //       close_conn = true;
//                //    }
//                //    break;
//                // }

               
//                // // Check to see if the connection has been closed by the client
//                // if (rc == 0) {
//                //    printf("[server] \tConnection closed\n");
//                //    close_conn = true;
//                //    break;
//                // }

//                // // Data was received 
//                // len = rc;
//                // printf("[server] \t%d bytes received\n", len);

            
//                SHASolver(fds[i].fd);

//                // Echo the data back to the client
//                // rc = send(fds[i].fd, buffer, len, 0);
//                // if (rc < 0) {
//                //    perror("[server] \tsend() failed");
//                //    close_conn = true;
//                //    break;
//                // }

//                // TEST 
//                // if (step % 2 == 0) {
//                //    sleep(5);
//                //    rc = send(fds[i].fd, buffer, len, 0);
//                // }
//                // else {
//                //    rc = send(fds[i].fd, buffer, len, 0);
//                // }
//                // ++step;

//                close(fds[i].fd);

//             } while(true);

//             // If the close_conn flag was turned on, we need to clean up this active connection. 
//             // This clean up process includes removing the descriptor.
//             if (close_conn) {
//                close(fds[i].fd);
//                fds[i].fd = -1;
//                compress_array = true;
//             }
//          }
//       }

//       if (compress_array) {
//          compress_array = false;
//          for (i = 0; i < nfds; i++) {
//             if (fds[i].fd == -1) {
//                for(j = i; j < nfds-1; j++) {
//                   fds[j].fd = fds[j+1].fd;
//                }
//                i--;
//                nfds--;
//             }
//          }
//       }
//    }
	
   
//    // Clean up all of the sockets that are open 
//    // maybe:)
//    for (i = 0; i < nfds; i++) {
//       if(fds[i].fd >= 0)
//          close(fds[i].fd);
//    }

//    printf("[server] Server ended successfully\n");
      
//    return 0;
// }













// int bruteForceHash(uint8_t hash[SHA256_DIGEST_LENGTH], uint8_t guessHash[SHA256_DIGEST_LENGTH]) {
//    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
//       if (hash[i] != guessHash[i])
//          return 0;
   
//    return 1;
// }

// void SHASolver(int conn_sd) {
//    // Read request from client
//    bzero(buffer, PACKET_REQUEST_SIZE);
//    if ((read(conn_sd, buffer, PACKET_REQUEST_SIZE)) == -1) {
//       perror("[server] read() failed");
//    exit(-1);
//    }

//    // Zerolize vars
//    request_t req = {0};

//    // Extrat data from request/msg
//    memcpy(&req.hash, buffer + PACKET_REQUEST_HASH_OFFSET, SHA256_DIGEST_LENGTH);
//    memcpy(&req.start, buffer + PACKET_REQUEST_START_OFFSET, 8);
//    memcpy(&req.end, buffer + PACKET_REQUEST_END_OFFSET, 8);
//    req.prio = buffer[PACKET_REQUEST_PRIO_OFFSET];

//    // Endianness 
//    req.start = be64toh(req.start);
//    req.end = be64toh(req.end);

//    #if DEBUG
//       printf("hash: ");
//       for (int i = SHA256_DIGEST_LENGTH - 1; i >= 0; --i)
//          printf("%02x", req.hash[i]);
//       printf("\n");

//       printf("start: %d\n", (int)req.start);
//       printf("end:   %d\n", (int)req.end);
//       printf("p:     %u\n", req.prio);
//    #endif 

//    // Brute force
//    uint8_t guessHash[SHA256_DIGEST_LENGTH];
//    response_t res = {0};
//    for (res.num = req.start; res.num < req.end; ++res.num) {
//       SHA256(res.bytes, sizeof(uint64_t), guessHash);
//       if (bruteForceHash(req.hash, guessHash)) {
//          res.num = htobe64(res.num);
//          break;
//       }
//    }

//    // Write response to client
//    if ((write(conn_sd, res.bytes, sizeof(uint64_t))) == -1) {
//       perror("[server] write() failed");
//       exit(-1);
//    }
// }