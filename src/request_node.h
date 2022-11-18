#ifndef NODE_REQ_H
#define NODE_REQ_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "os-challenge-util.h"

// Node to store request in a data structure
typedef struct node_req{
    request_t *req;
    struct node_req *next;
}node_req_t;

#endif // NODE_REQ_H