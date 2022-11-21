#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/ioctl.h>



struct Server server_constructor(int domain, 
                                 int service, 
                                 int protocol, 
                                 u_long interface, 
                                 int port, 
                                 int backlog, 
                                 blocking_mode block_mode)
{
    struct Server server; 

    server.domain = domain;
    server.service = service; 
    server.protocol = protocol; 
    server.interface = interface;
    server.port = port;
    server.backlog = backlog;
    server.block_mode = block_mode;

    server.address.sin_family = domain;
    server.address.sin_addr.s_addr = htons(interface);
    server.address.sin_port = htons(port);

    if ((server.socketfd = socket(domain, service, protocol)) < 0) {
        perror("[server][!] (server_construtor) socket() failed");
        exit(EXIT_FAILURE);
    }

    // Set socket option to reuse the addr/port without waiting for it to be release 
    int optVal = 1;
    if ((setsockopt(server.socketfd, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal))) < 0) {
        perror("[server][!] (server_construtor) setsockopt() failed");
        exit(EXIT_FAILURE);
    }

    if ((bind(server.socketfd, (struct sockaddr *) &server.address, sizeof(server.address))) < 0) {
        perror("[server][!] (server_construtor) bind() failed");
        exit(EXIT_FAILURE);
    }

    // Set socket blocking mode
    if (block_mode == NONBLOCKING) {
        if ((ioctl(server.socketfd, FIONBIO, (char *)&optVal)) < 0) {
	    	perror("[server][!] (server_construtor) ioctl() failed");
            exit(EXIT_FAILURE);
	    }
    }

    if ((listen(server.socketfd, server.backlog)) != 0) {
        perror("[server][!] (server_construtor) listen() failed");
        exit(EXIT_FAILURE);
    }
    printf("[server][*] Listening on port %d...\n", port);

    return server; 
}