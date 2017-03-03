#ifndef COMMANDQUEUE_H
#define COMMANDQUEUE_H

#include<stdio.h>
#include<stdlib.h>
#include <stdbool.h>

typedef struct
{
    char message[20];
} Command;

typedef struct CommandQueue
{
    int capacity;
    int size;
    int front;
    int rear;
    Command *elements;
} CommandQueue;

CommandQueue * create_command_queue(int max);

void dequeue(CommandQueue *q);

Command front(CommandQueue *q);

void enqueue(CommandQueue *q, Command element);

bool is_empty(CommandQueue *q);

#endif
