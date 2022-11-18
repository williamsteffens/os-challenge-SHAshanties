#pragma once 

#include "server.h"



void launch_fork_per_req_server(struct Server *server);

void launch_preforked_server(struct Server *server, short nprocesses);