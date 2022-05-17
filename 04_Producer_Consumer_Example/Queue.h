#ifndef QUEUE_H
#define QUEUE_H

#include "pthread.h"
#include "stdbool.h"

#define Q_DEFAULT_SIZE  5

struct Queue_t
{
    unsigned int forward;
    unsigned int rear;
    unsigned int count;
    void* elements[Q_DEFAULT_SIZE];
    pthread_mutex_t q_mutex;
    pthread_cond_t q_cond;
};

struct Queue_t* create(void);

bool isqueueempty(struct Queue_t* queue);

bool isqueuefull(struct Queue_t* queue);

bool enqueue(struct Queue_t* queue, void* ptr);

void* dequeue(struct Queue_t* queue);

void print_Queue(struct Queue_t* queue);

#define Q_COUNT(q)  (q->count)


#endif