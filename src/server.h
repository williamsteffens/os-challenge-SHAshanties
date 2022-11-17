/*
    server.h
*/

#pragma once

#include <sys/socket.h>
#include <netinet/in.h>

typedef enum
{
    BLOCKING,
    NONBLOCKING
} blocking_mode;

struct Server
{
    int domain;
    int service; 
    int protocol; 
    u_long interface; 
    int port; 
    int backlog; 
    blocking_mode block_mode;

    struct sockaddr_in address;

    int socketfd;
};

struct Server server_constructor(int domain, int service, int protocol, u_long interface, int port, int backlog, blocking_mode block_mode);

