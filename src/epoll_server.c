#define DEBUG 0

#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
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


#include "os-challenge-util.h"
#include "server.h"

#include "epoll_server.h"

#define MAX_CONN        100
#define MAX_EVENTS      100


/*
 * register events of fd to epfd
 */
static void epoll_ctl_add(int epfd, int fd, uint32_t events)
{
	struct epoll_event ev;
	ev.events = events;
	ev.data.fd = fd;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		perror("[server] epoll_ctl() failed ");
		exit(1);
	}
}

void SHASolver(int conn_sd) 
{
    uint8_t buffer[PACKET_REQUEST_SIZE];

    // Read request from client
    bzero(buffer, PACKET_REQUEST_SIZE);
    if ((read(conn_sd, buffer, PACKET_REQUEST_SIZE)) == -1) {
        perror("[server] read() failed");
    exit(-1);
    }

    // Zerolize vars
    request_t req = {0};

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
    response_t res = {0};
    for (res.num = req.start; res.num < req.end; ++res.num) {
        SHA256(res.bytes, sizeof(uint64_t), guessHash);
        if (hashMatches(req.hash, guessHash)) {
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



void launch_epoll_server(struct Server *server)
{
	int epfd, nfds;
	int conn_sd, socklen;
	uint8_t buffer[PACKET_REQUEST_SIZE];
	struct sockaddr_in cli_addr;
	struct epoll_event events[MAX_EVENTS];

    if ((epfd = epoll_create1(0)) == -1) {
        perror("[server][!] epoll_create1() failed \n");
        close(server->socket);
        exit(EXIT_FAILURE);
    }

    epoll_ctl_add(epfd, server->socket, EPOLLIN);

    socklen = sizeof(cli_addr);

    // TODO: ERROR CHECKING!!!!!!!!!

    for (;;) 
    {
        if ((nfds = epoll_wait(epfd, events, MAX_EVENTS, -1)) == -1) {
            perror("[server][!] epoll_create1() failed \n");
            close(server->socket);
            exit(EXIT_FAILURE);
        }

		for (int fd = 0; fd < nfds; ++fd) {
            if (events[fd].events & EPOLLIN) {
                if (events[fd].data.fd == server->socket) {
                    /* handle new connection */
                    if ((conn_sd = accept(server->socket, (struct sockaddr *)&cli_addr, &socklen)) < 0) {
                        printf("[server][!] accept() failed \n");
                        close(conn_sd);
                    }

                    #if DEBUG 
                        inet_ntop(AF_INET, (char *)&(cli_addr.sin_addr), buffer, sizeof(cli_addr));
                        printf("[server][+] Connected with %s:%d\n", buffer, ntohs(cli_addr.sin_port));
                    #endif

                    epoll_ctl_add(epfd, conn_sd, EPOLLIN); //  | EPOLLET | EPOLLRDHUP | EPOLLHUP
			    } 
                else {
                    /* handle EPOLLIN event */
                    
                    //request_t req = {0};
                    //bzero(buffer, sizeof(buffer));

                    // if ((recv(events[fd].data.fd, buffer, sizeof(buffer), 0)) <= 0 /* || errno == EAGAIN */ ) {
                    // 	printf("[server] All incoming connections have been processed.\n");
                    //     break;
                    // } 

                    //display_request(buffer, req);

                    SHASolver(events[fd].data.fd);

                    //send(events[fd].data.fd, buffer, strlen(buffer), 0);

                    close(events[fd].data.fd);
                }
            }
            else {
				printf("[server][!] unexpected\n");
			}

			/* check if the connection is closing */
			// if (events[fd].events & (EPOLLRDHUP | EPOLLHUP)) {
			// 	printf("[+] connection closed\n");
			// 	epoll_ctl(epfd, EPOLL_CTL_DEL, events[fd].data.fd, NULL);
			// 	close(events[fd].data.fd);
			// 	continue;
			// }
		}
    }
}