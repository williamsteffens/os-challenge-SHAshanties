#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "server.h"
#include "epoll_server.h"
#include "threaded_server.h"
#include "server_I.h"
#include "server_II.h"



int main(int argc, char *argv[]) 
{
    if (argc != 2) {
      fprintf(stderr, "Incorrect usage. Usage: %s port\n", argv[0]);
      exit(EXIT_FAILURE);
    } 

    // Second Argument should be port num
    short port = (unsigned short) atoi(argv[1]);

    struct Server server = server_constructor(AF_INET, SOCK_STREAM, 0, INADDR_ANY, port, 1000, BLOCKING);
    //launch_server_I(&server, 4, 4);
    
    launch_server_II(&server, 4);
    //launch_epoll_server(&server);
    //launch_poll_server(&server);
    //launch_select_server(&server);


    //launch_thread_per_client_server(&server);
    //launch_thread_pool_busy_wait_server(&server);
    //launch_thread_pool_server(&server);
    //launch_x_threads_one_client_server(&server, 4);
    //launch_x_threads_one_client_server_many_tasks_server(&server, 4); // sysconf(_SC_NPROCESSORS_CONF)
}
