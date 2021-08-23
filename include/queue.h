#ifndef QUEUE
#define QUEUE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

typedef struct
{
    void (*function)(void *);
    void *argument;
} queue_task_t;

typedef struct node_struct
{
    void *data;
    bool destroy;
    struct node_struct *next;
    struct node_struct *prev;
    queue_task_t *task;
    pthread_mutex_t lock;
    pthread_cond_t notify;
    pthread_t thread_id;
    bool is_running;
} Node;

typedef struct queue_struct
{

    pthread_mutex_t lock;
    pthread_cond_t notify;

    unsigned size_queue;
    Node *head;
    Node *tail;
    size_t mem_size;
    int no_ocuped;

} Queue;

Queue *crear_queue(size_t mem_size);

int enqueue(Queue *q, const void *data);
Node *dequeue(Queue *q);
void clear_queue(Queue *q);
unsigned get_size(Queue *q);
static void *task_thread(void *data);


#endif /*QUEUE*/