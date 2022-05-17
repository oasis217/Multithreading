#include "Queue.h"
#include "stdlib.h"
#include "stdio.h"

//----------------------------------------------
struct Queue_t* create(void)
{
    struct Queue_t* queue = (struct Queue_t*)malloc(sizeof(struct Queue_t));
    queue->count = 0;
    queue->forward = 0;
    queue->rear=0;
    pthread_mutex_init(&queue->q_mutex,NULL);
    pthread_cond_init(&queue->q_cond,NULL);

    return queue;
}
//----------------------------------------------
bool isqueueempty(struct Queue_t* queue)
{
    if(queue->count == 0)
        return true;
    return false;
}

//----------------------------------------------
bool isqueuefull(struct Queue_t* queue)
{
    if(queue->count == Q_DEFAULT_SIZE)
        return true;
    return false;
}

//----------------------------------------------
bool enqueue(struct Queue_t* queue, void* ptr)
{
      if(!queue)
      {
        printf("Queue is NULL pointer\n");
        return false;
      }

      if(!ptr)
      {
          printf("Value is NULL\n");
          return false;
      }
    //--
      if(isqueuefull(queue))
      {
        printf("Queue is full, cannot insert further element\n");
        return false;
      }

    //-- 
        if(isqueueempty(queue))  //first element
        {
            queue->elements[queue->forward]=ptr;
            queue->count++;
            return true;
        }
              
    //--
      if(queue->rear  == (Q_DEFAULT_SIZE-1))
      {
        queue->rear = 0;
      }
      else
      {
        queue->rear += 1;
      }

        queue->elements[queue->rear] = ptr;
        queue->count++;


    return true;

}

//----------------------------------------------
void* dequeue(struct Queue_t* queue)
{
    if(!queue)
    {
        printf("Queue is NULL pointer\n");
        return NULL;
    }

    if(isqueueempty(queue))
    {
        printf("Queue is empty\n");
        return NULL;
    }

    void* x = queue->elements[queue->forward];
    queue->elements[queue->forward] = NULL;
    queue->count--;

    if(queue->forward == queue->rear)
    {
        return x;
    }

    if(queue->forward  == (Q_DEFAULT_SIZE-1))
        queue->forward = 0;
    else
        queue->forward += 1;
  
    return x;
}

//----------------------------------------------
void print_Queue(struct Queue_t* queue)
{
    unsigned int i= 0;

    printf("q->front = %d, q->rear = %d, q->count = %d\n",queue->forward,queue->rear,queue->count);

    for(i=0; i<(Q_DEFAULT_SIZE); i++)
    {
        if(queue->elements[i] == NULL)
            continue;

        printf("index = %u, element = %p\n",i,queue->elements[i]);
    }

}