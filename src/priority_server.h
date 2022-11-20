#pragma once 



void launch_priority_cached_thread_pool_server(struct Server *server, int nthreads);

void *thread_pool_worker_priority_cached();