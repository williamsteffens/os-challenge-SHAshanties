#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "server.h"
#include "threaded_server.h"
#include "os-challenge-util.h"
#include "forked_server.h"
#include "nonblocking_IO_server.h"
#include "cached_server.h"
#include "co_worker.h"



int main(int argc, char *argv[]) 
{
    if (argc != 2) {
        fprintf(stderr, "Incorrect usage. Usage: %s port\n", argv[0]);
        exit(-1);
    }

    // Second Argument should be port num
    short port = (unsigned short) atoi(argv[1]);

    struct Server server = server_constructor(AF_INET, SOCK_STREAM, 0, INADDR_ANY, port, 1000, BLOCKING);

    // Forked servers
    // ------------------------------
    //launch_fork_per_req_server(&server);
    //launch_preforked_server(&server, 4);

    // Threaded servers
    // ------------------------------
    //launch_thread_per_client_server(&server);
    //launch_thread_pool_server(&server, 4);

    // Cached server
    // ------------------------------
    //launch_cached_thread_pool_server(&server, 4);

    // Split Request server
    // ------------------------------
    //launch_split_req_cached_thread_pool_server(&server, 4);

    // Nonblocking I/O server
    // ------------------------------
    //launch_nonblocking_IO_cached_thread_pool_server(&server, 4);
    launch_nonblocking_IO_cached_split_thread_pool_server(&server, 4);

    // Priority server
    // ------------------------------

    // CPU tuned affinity server

}
