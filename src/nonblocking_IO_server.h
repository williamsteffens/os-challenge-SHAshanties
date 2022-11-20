#pragma once



pthread_mutex_t done_queue_mutex;


void launch_nonblocking_IO_cached_thread_pool_server(struct Server *server, int nthreads);

void launch_nonblocking_IO_cached_split_thread_pool_server(struct Server *server, int nthreads);


void *thread_pool_nonblocking_IO_cached_worker();

void *thread_pool_nonblocking_IO_cached_split_worker();