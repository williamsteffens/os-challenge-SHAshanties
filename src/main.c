#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "server.h"
#include "threaded_server.h"
#include "server_I.h"
#include "server_II.h"
#include "os-challenge-util.h"
#include "forked_server.h"
#include "coworker.h"
#include "nonblocking_IO_server.h"
#include "cached_server.h"



int main(int argc, char *argv[]) 
{
    if (argc != 2) {
        fprintf(stderr, "Incorrect usage. Usage: %s port\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Second Argument should be port num
    short port = (unsigned short) atoi(argv[1]);

    struct Server server = server_constructor(AF_INET, SOCK_STREAM, 0, INADDR_ANY, port, 1000, BLOCKING);


    //launch_co_worker_server(&server, 4);
    //launch_server_II(&server, 4);

    // Forked servers
    // ------------------------------
    //launch_fork_per_req_server(&server);
    //launch_preforked_server(&server, 4);
    // ------------------------------

    // Threaded servers
    // ------------------------------
    //launch_thread_per_client_server(&server);
    //launch_thread_pool_server(&server, 4);


    //launch_nthreads_one_client_server(&server, 4);
    //launch_nthreads_one_client_server_many_tasks_server(&server, 4); // sysconf(_SC_NPROCESSORS_CONF)
    // ------------------------------

    // Split Request server
    // launch_split_req_thread_pool_server(&server, 4);

    // Nonblocking I/O server
    //launch_nonblocking_IO_split_req_thread_pool_server(&server, 4);

    // Cached server
    launch_cached_split_req_thread_pool_server(&server, 4);

    // Priority server


    //launch_server_I(&server, 4, 4);
    

    //launch_server_II(&server, 4);


}
