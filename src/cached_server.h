#pragma once 


pthread_mutex_t htable_mutex;


void launch_cached_split_req_thread_pool_server(struct Server *server, int nthreads);