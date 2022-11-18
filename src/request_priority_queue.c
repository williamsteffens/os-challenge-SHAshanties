#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "os-challenge-util.h"
#include "request_node.h"
#include "request_queue.h"
#include "request_priority_queue.h"

priority_queue_req_t init_pq(int size){
    //TODO: Create a constructor of size multiple PQ's.
    // Each PQ holds requests with coorosponding priority -1
    priority_queue_req_t pq;
    pq.size = size;
    pq.queues = (queue_req_t*)malloc(size * sizeof(queue_req_t*)); 
}

void enqueue_pq(priority_queue_req_t pq, request_t *req){
    //TODO: method for inserting request according to priority
    // The request is inserted into a pq by priority
    

    //int priority = req->prio -1; // Priority = {1...size}, array = {0...size-1}
    //enqueue_req(req); 
    //pq.req_node[priority];
}
    
request_t *dequeue_pq(){
    //TODO: Dequeue from highest non-empty PQ 
    // Additional: update PQ's to shift request to higher PQ with time

}

// Driver code
int main(void){
    priority_queue_req_t pq = init_pq(16);
    printf("%d\n", (pq.queues[0].head));
    printf("%d", &(pq.queues[0].head));
    printf("%d\n", (pq.queues[1].head));
    printf("%d", &(pq.queues[1].head));
    return 0;
}

/* Experiments:
    - Test naive approach, F.I.F.S., i.e. priority 1 stays in PQ[0] until all others are empty
    - Test timed priority, i.e. request priority increases with time, pushing onto higher PQs
 
::LINKS::
"1 queue for each priority"
* https://stackoverflow.com/questions/2921349/priority-queue-with-dynamic-priorities
"Priority scheduling"
* https://www.scaler.com/topics/operating-system/priority-scheduling-algorithm/
*/