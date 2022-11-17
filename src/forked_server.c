#include "os-challenge-util.h"
#include "server.h"
#include "brute_force.h"
#include "simple_queue.h"

pthread_mutex_t queue_mutex_fork = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond_var_fork = PTHREAD_COND_INITIALIZER;

void launch_fork_per_req_server(struct Server *server)
{
    int conn_sd, socklen;
    pid_t pid;
    struct sockaddr_in cli_addr;
    socklen = sizeof(cli_addr);

    for (;;) {
        if ((conn_sd = accept(server->socketfd, (struct sockaddr *)&cli_addr, &socklen)) < 0) {
            perror("[server][!] accept() failed\n");
            close(conn_sd);
            exit(-1);
        }

        if ((pid = fork()) < 0) {
            perror("[server][!] fork() failed\n");
            exit(-1);
        }

        if (pid == 0) {
            close(server->socketfd);
            brute_force_SHA(conn_sd);
            exit(0);
        }
        else {
            close(conn_sd);
        }
    }
}

void launch_preforked_server(struct Server *server, short nprocesses)
{
    int conn_sd, socklen;
    pid_t pid;
    struct sockaddr_in cli_addr;
    socklen = sizeof(cli_addr);

    for (int i = 0; i < nprocesses; ++i) {
        if ((pid = fork()) < 0) {
            perror("[server][!] fork() failed");
            exit(-1);
        }

        if (pid == 0) {
            // Child process server loop
            for (;;) {
                if ((conn_sd = accept(server->socketfd, (struct sockaddr *)&cli_addr, &socklen)) < 0) {
                    perror("[server][!] accept() failed\n");
                    close(conn_sd);
                    exit(-1);
                }

                brute_force_SHA(conn_sd);
            }
        }
    }

    // Pause spawner process
    pause();
}