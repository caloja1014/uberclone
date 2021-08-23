#include "../include/queue.h"

Queue *crear_queue(size_t mem_size)
{
    Queue *queue = (Queue *)malloc(sizeof(Queue));

    queue->mem_size = mem_size;
    queue->head = queue->tail = NULL;
    queue->size_queue = queue->no_ocuped = 0;
    pthread_mutex_init(&queue->mutex_dequeue, NULL);
    pthread_cond_init(&queue->cond_dequeue, NULL);
    return queue;
}
int enqueue(Queue *q, const void *data)
{
    pthread_mutex_lock(&q->mutex_dequeue);
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL)
    {
        return -1;
    }
    new_node->data = malloc(q->mem_size);
    new_node->task = (queue_task_t *)malloc(sizeof(queue_task_t));
    if (new_node->data == NULL)
    {
        free(new_node);
        return -1;
    }
    new_node->next = NULL;
    new_node->prev = NULL;
    memcpy(new_node->data, data, q->mem_size);

    if (q->size_queue == 0)
    {
        q->head = q->tail = new_node;
    }
    else
    {
        q->tail->next = new_node;
        new_node->prev = q->tail;
        q->tail = new_node;
    }
    q->no_ocuped++;
    q->size_queue++;
    pthread_mutex_init(&new_node->lock, NULL);
    pthread_cond_init(&new_node->notify, NULL);
    new_node->is_running = new_node->destroy = false;
    pthread_create(&new_node->thread_id, NULL, task_thread, (void *)new_node);
    // pthread_join(new_node->thread_id, NULL);

    
    pthread_mutex_unlock(&q->mutex_dequeue);
    pthread_cond_broadcast(&q->cond_dequeue);
    return 0;
}
Node *dequeue(Queue *q)
{
    pthread_mutex_lock(&q->mutex_dequeue);
    if (q->size_queue > 0)
    {
        Node *temp = q->head;
        if (q->size_queue == 2)
        {
            q->head = q->tail = q->head->next;
            q->head->prev = q->tail->prev = NULL;
            q->tail->next = NULL;
        }else if (q->size_queue > 2){
            q->head = q->head->next;
            q->head->prev  = NULL;
            q->tail->next = NULL;
        }
        else
        {
            q->head = NULL;
            q->tail = NULL;
        }
        

        q->size_queue--;
        pthread_mutex_unlock(&q->mutex_dequeue);
        pthread_cond_broadcast(&q->cond_dequeue);
        return temp;
    }
    pthread_mutex_unlock(&q->mutex_dequeue);
    pthread_cond_broadcast(&q->cond_dequeue);
    return NULL;
}

static void *task_thread(void *data)
{
    Node *node = (Node *)data;

    for (;;)
    {

        pthread_mutex_lock(&node->lock);
        while (!node->is_running && !node->destroy)
        {
            pthread_cond_wait(&node->notify, &node->lock);
        }
        node->is_running = false;
        pthread_mutex_unlock(&node->lock);
        (*(node->task->function))(node->task->argument);
    }
}
void clear_queue(Queue *q)
{
    Node *temp;
    while (q->size_queue > 0)
    {
        temp = q->head;
        q->head = temp->next;
        free(temp->data);
        free(temp);
        q->size_queue--;
    }
    q->head = q->tail = NULL;
}

unsigned get_size(Queue *q)
{
    return q->size_queue;
}
