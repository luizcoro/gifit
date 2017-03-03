#include "commandqueue.h"

CommandQueue * create_command_queue(int max)
{
    CommandQueue *q;
    q = (CommandQueue*) malloc(sizeof(CommandQueue));

    q->elements = (Command *)malloc(sizeof(Command) * max);
    q->size = 0;
    q->capacity = max;
    q->front = 0;
    q->rear = -1;

    return q;
}

void dequeue(CommandQueue *q)
{
    if(q->size==0)
        return;

    q->size--;
    q->front++;

    if(q->front == q->capacity)
        q->front=0;
}

Command front(CommandQueue *q)
{
    if(q->size==0)
        exit(1);

    return q->elements[q->front];
}

void enqueue(CommandQueue *q, Command element)
{
    q->size++;
    q->rear++;

    if(q->rear == q->capacity)
        q->rear = 0;

    q->elements[q->rear] = element;
}

bool is_empty(CommandQueue *q)
{
    return (q->size == 0);
}
