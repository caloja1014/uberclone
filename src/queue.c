#include "../include/queue.h"

Queue *crear_queue(size_t mem_size)
{
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    queue->mem_size = mem_size;
    queue->head = queue->tail = NULL;
    queue->size_queue = 0;
    return queue;
}

int enqueue(Queue *q, const void *data)
{
    
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL)
    {
        return -1;
    }
    new_node->data = malloc(q->mem_size);

    if (new_node->data == NULL)
    {
        free(new_node);
        return -1;
    }
    new_node->next = NULL;
    memcpy(new_node->data, data, q->mem_size);

    if (q->size_queue == 0)
    {
        q->head = q->tail = new_node;
    }
    else
    {
        q->tail->next = new_node;
        q->tail = new_node;
    }
    q->size_queue++;
    return 0;
}
void dequeue(Queue *q){
    if (q->size_queue > 0){
        Node *temp=q->head;
        if (q->size_queue >1){
            q->head=q->head->next;
        }else{
            q->head=NULL;
        }
        q->size_queue--;
        free(temp->data);
        free(temp);
    }
}
void clear_queue (Queue *q){
    Node *temp;
    while(q->size_queue>0){
        temp=q->head;
        q->head=temp->next;
        free(temp->data);
        free(temp);
        q->size_queue--;
    }
    q->head=q->tail=NULL;
}

unsigned get_size(Queue *q){
    return q->size_queue;
}
