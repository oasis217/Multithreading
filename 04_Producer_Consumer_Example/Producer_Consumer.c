/*
Problem Statement :
====================
The program  launches 4 threads - 2 consumer threads and 2 producer threads. Threads
are created in JOINABLE Mode.
All 4 threads act on a shared resource - A Queue of integers. Producer threads produce
a random integer and add it to Queue, Consumer threads remove an integer from the Queue.
Maximum size of the Queue is 5.

Following are the constraints applied :

1. When producer threads produce an element and add it to the Queue, it does not release the Queue
untill the Queue is full i.e producer thread release the Queue only when it is full

2. When consumer threads consume an element from the Queue, it consumes the entire Queue and
do not release it until the Queue is empty.

3. Consumer Signals the Producers when Queue is Exhausted, Producers Signals the Consumers when Queue	
becomes full

*/

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include"Queue.h"

//-----------------------------------
//-- Function creates integer values
static int new_int()
{
    static int a=0;
    a++;
    return a;
}

struct Queue_t* queue = NULL;

static const char* prod1 = "TP1";
static const char* prod2 = "TP2";
static const char* prod3 = "TP3";
static const char* cons1 = "TC1";
static const char* cons2 = "TC2";
static const char* cons3 = "TC3";

//---------------------------------------------------------------
static void* prod_func(void* arg)
{
    char* thread = (char*)arg;
    bool status;
    int value;

   
    
    pthread_mutex_lock(&queue->q_mutex);

    printf("Thread %s started its execution\n",thread);

    while(isqueuefull(queue))
    {
        printf("Thread %s is waiting for the signal\n",thread);
        printf("\n");
        pthread_cond_wait(&queue->q_cond,&queue->q_mutex);
        printf("Thread %s got the signal and restarting its execution\n",thread);
    }

    for(int i=0; i<(Q_DEFAULT_SIZE); i++)
    {
        value = new_int();
        status = enqueue(queue,(void*)value);
        if(!status)
            printf("Error inserting elements in queue\n");

        printf("Thread %s inserted element %d\n",thread,value);
    }

    pthread_cond_broadcast(&queue->q_cond);

    printf("Thread %s finished its execution\n",thread);
    printf("\n");
    pthread_mutex_unlock(&queue->q_mutex);


    return NULL;
}

//---------------------------------------------------------------
static void* cons_func(void* arg)
{
    char* thread = (char*)arg;
    int value;

    pthread_mutex_lock(&queue->q_mutex);

    printf("Thread %s started its execution\n",thread);

    while(isqueueempty(queue))
    {
        printf("Thread %s is waiting for the signal\n",thread);
        printf("\n");
        pthread_cond_broadcast(&queue->q_cond,&queue->q_mutex);
        printf("Thread %s got the signal and restarting its execution\n",thread);
    }

    

    for(int i=0; i<(Q_DEFAULT_SIZE); i++)
    {
        value = (int)dequeue(queue);       
        printf("Thread %s removed element %d\n",thread,value);

    }

    pthread_cond_signal(&queue->q_cond);
    printf("Thread %s finished its execution\n",thread);
    printf("\n");
    pthread_mutex_unlock(&queue->q_mutex);

    return NULL;
}



int main(int argc,char** argv)
{
    queue = create();
    
    //print_Queue(queue);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);

    pthread_t prod_thr1,prod_thr2,prod_thr3;
    pthread_t cons_thr1,cons_thr2,cons_thr3;

    pthread_create(&prod_thr1,&attr,prod_func,(void*)prod1);
    pthread_create(&prod_thr2,&attr,prod_func,(void*)prod2);
    pthread_create(&prod_thr3,&attr,prod_func,(void*)prod3);
    pthread_create(&cons_thr1,&attr,cons_func,(void*)cons1);
    pthread_create(&cons_thr2,&attr,cons_func,(void*)cons2);
    pthread_create(&cons_thr3,&attr,cons_func,(void*)cons3);

    pthread_join(prod_thr1,0);
    pthread_join(prod_thr2,0);
    pthread_join(prod_thr3,0);
    pthread_join(cons_thr1,0);
    pthread_join(cons_thr2,0);
    pthread_join(cons_thr3,0);

    printf("\n");
    printf("Printing Queue\n");
    print_Queue(queue);
    printf("\n");
    
    printf("Program Finished!\n");
    pthread_exit(0);
    return 0;
}

