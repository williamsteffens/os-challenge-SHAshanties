#include <stdio.h>
#include <stdlib.h>

// TODO: Change type of "data" to request

struct node{
    struct node *prev;
    struct node *next;
    int data;
};

struct pQueue{
    struct node **head;
    struct node **tail;
};


// Pushes data to head/front of queue
void pushHead(struct node** head, int newData){

    // Create new node with data
    struct node* newNode = (struct node*)malloc(sizeof(struct node));
 
    // Insert data into node object
    newNode->data = newData;
    
    // Set prev of newNode to NULL
    newNode->prev = NULL;

    // Set next of newNode to head
    newNode->next = (*head);
    
    // If head not NULL, Set newNode as prev to head
    if ((*head) != NULL){
        (*head)->prev = newNode;
    }
    
    // Update the head to point to the new node
    (*head) = newNode;
}

// Pushes data to tail/back of queue
void pushTail(struct node** tail, int newData){

    // Create new node with data
    struct node* newNode = (struct node*)malloc(sizeof(struct node));
 
    // Insert data into node object
    newNode->data = newData;
    
    // Set next of newNode to NULL
    newNode->next = NULL;

    // Set prev of newNode to head
    newNode->prev = (*tail);
    
    // If tail not NULL, Set newNode as next to head
    if ((*tail) != NULL){
        (*tail)->next = newNode;
    }
    
    // Update the head to point to the new node
    (*tail) = newNode;
}

// Print out priority queue
void printPQueue(struct pQueue* pq){
    // Create pointer to travers the queue
    struct node* currentNode = pq->head;

    // Traverse and print the queue, starting at the head
    printf("\nPopping from front of queue\n");
    while(currentNode != NULL) {
        printf("%d ", currentNode->data);
        currentNode = currentNode->next;
    }

    // Traverse and print the queue, starting at the tail    
    currentNode = pq->tail;
    printf("\nPopping from back of queue\n");
    while(currentNode != NULL) {
        printf("%d ", currentNode->data);
        currentNode = currentNode->prev;
    }

    // Free the current node pointer
    free(currentNode);
}


// Test code
int main(){
    //TODO: Currently having pointer mismatch

    struct pQueue* pq = NULL;

    pushHead(pq->head, 1);
    pushHead(pq->head, 2);
    pushHead(pq->head, 3);
    pushHead(pq->head, 4);
    pushHead(pq->head, 5);

    pushTail(pq->tail, 1);
    pushTail(pq->tail, 2);
    pushTail(pq->tail, 3);
    pushTail(pq->tail, 4);
    pushTail(pq->tail, 5);

    printPQueue(pq);

    return 0;
}