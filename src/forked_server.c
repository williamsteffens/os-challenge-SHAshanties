#include "os-challenge-util.h"
#include "server.h"
#include "brute_force.h"



void launch_fork_per_req_server(struct Server *server)
{
    int conn_sd, socklen;
    pid_t pid;
    struct sockaddr_in cli_addr;
    socklen = sizeof(cli_addr);

    for (;;) {
        if ((conn_sd = accept(server->socket, (struct sockaddr *)&cli_addr, &socklen)) < 0) {
            perror("[server][!] accept() failed");
            close(conn_sd);
            exit(-1);
        }

        if ((pid = fork()) < 0) {
            perror("[server][!] fork() failed");
            exit(-1);
        }

        if (pid == 0) {
            close(server->socket);
            brute_force_SHA(conn_sd);
            exit(0);
        }
        else {
            close(conn_sd);
        }
    }
}