#define DEBUG 0

#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/select.h>
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
#include <fcntl.h>
#include <math.h>

#include "os-challenge-util.h"
#include "server.h"
#include "brute_force.h"
#include "threaded_server.h"
#include "simple_queue.h"
#include "os-challenge-util.h"
#include "split_request_server.h"


#define MAX_CONN        1000
#define MAX_EVENTS      1000


int epollfd;


static void epoll_ctl_add(int epollfd, int fd, uint32_t events)
{
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = events;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		perror("[server][!] epoll_ctl() failed ");
		exit(-1);
	}
}


void launch_nonblocking_IO_split_req_thread_pool_server(struct Server *server, int nthreads)
{
    int conn_sd, socklen;
    int nfds;
    struct epoll_event events[MAX_EVENTS];
    int req_id = 0;
    unsigned int chunk;
    struct sockaddr_in cli_addr;
    socklen = sizeof(cli_addr);
    uint8_t buffer[PACKET_REQUEST_SIZE];
    request_t req;

    pthread_mutex_init(&queue_mutex, NULL);
    pthread_cond_init(&queue_cond_var, NULL);

    if ((epollfd = epoll_create1(0)) == -1) {
        perror("[server][!] epoll_create1() failed \n");
        close(server->socketfd);
        exit(-1);
    }

    epoll_ctl_add(epollfd, server->socketfd, EPOLLIN);

    pthread_t thread_pool[nthreads];
    for (int i = 0; i < nthreads; ++i)
        pthread_create(&thread_pool[i], NULL, thread_pool_worker, NULL);

    // Server loop
    for (;;) {
        if ((nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1)) == -1) {
            perror("[server][!] epoll_wait() failed \n");
            close(server->socketfd);
            exit(-1);
        }

        // Iterate monitored sockets
        for (int i = 0; i < nfds; ++i) {
            int sd = events[i].data.fd;
            if (sd == server->socketfd) {
                // Handle new connection
                if ((conn_sd = accept(server->socketfd, (struct sockaddr *)&cli_addr, &socklen)) < 0) {
                    perror("[server][!] accept() failed \n");
                    close(conn_sd);
                } 

                fcntl(conn_sd, F_SETFD, fcntl(conn_sd, F_GETFD, 0) | O_NONBLOCK);

                #if DEBUG 
                    inet_ntop(AF_INET, (char *)&(cli_addr.sin_addr), buffer, sizeof(cli_addr));
                    printf("[server][+] Connected with %s:%d\n", buffer, ntohs(cli_addr.sin_port));
                #endif

                epoll_ctl_add(epollfd, conn_sd, EPOLLIN);
            } 
            else if (events[i].events & EPOLLIN) {
                // Handle EPOLLIN event
                bzero(buffer, PACKET_REQUEST_SIZE);
                if ((read(sd, buffer, PACKET_REQUEST_SIZE)) == -1) {
                    perror("[server][!] read() failed\n");
                    exit(-1);
                }
            
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

                split_and_sumbit_task(nthreads, conn_sd, req_id, req.hash, req.start, req.end);

                // Inc req_id for next request
                ++req_id;

                epoll_ctl(epollfd, EPOLL_CTL_DEL, sd, NULL);
            }
        }
    }
}