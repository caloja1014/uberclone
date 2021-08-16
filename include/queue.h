#ifndef QUEUE
#define QUEUE
#include <stdio.h>

typedef struct node_struct{
    void *data;
    struct node_struct *next;
} Node;

typedef struct queue_struct{
    unsigned size_queue;
    Node *head;
    Node *tail;
    size_t mem_size;
} Queue;

Queue * crear_queue(size_t mem_size);

int enqueue(Queue *q, void *data);
void dequeue(Queue *q);
void clear_queue (Queue *q);
unsigned get_size(Queue *q);

#endif /*QUEUE*/