#pragma once

void launch_thread_per_client_server(struct Server *server);
void launch_thread_per_client_innate_prio_server(struct Server *server);
void launch_thread_pool_busy_wait_server(struct Server *server);
void launch_thread_pool_server(struct Server *server);
