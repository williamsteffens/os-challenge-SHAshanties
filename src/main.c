#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "server.h"
#include "epoll_server.h"
#include "threaded_server.h"



int main(int argc, char *argv[]) 
{
    if (argc != 2) {
      fprintf(stderr, "Incorrect usage. Usage: %s port\n", argv[0]);
      exit(EXIT_FAILURE);
    } 

    // Second Argument should be port num
    short port = (unsigned short) atoi(argv[1]);

    struct Server server = server_constructor(AF_INET, SOCK_STREAM, 0, INADDR_ANY, port, 100, BLOCKING);
    //launch_epoll_server(&server);
    //launch_poll_server(&server);
    //launch_select_server(&server);


    //launch_thread_per_client_server(&server);
    //launch_thread_pool_busy_wait_server(&server);
    launch_thread_pool_server(&server);
}
