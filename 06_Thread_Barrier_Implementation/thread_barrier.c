#include<stdio.h>
#include "thread_barrier.h"

void thread_barrier_print(th_barrier_t* barrier)
{
    printf("barrier->threshold_count : %u\n",barrier->threshold_count);
    printf("barrier->curr_wait_count : %u\n",barrier->curr_wait_count);
    printf("barrier->is_ready_again : %u\n",barrier->is_ready_again);
}

void thread_barrier_init(th_barrier_t* barrier, uint32_t threshold_count)
{
    barrier->threshold_count = threshold_count;
    barrier->curr_wait_count = 0;
    barrier->is_ready_again = true;
    pthread_mutex_init(&barrier->br_mutex,NULL);
    pthread_cond_init(&barrier->br_cv,NULL);
    pthread_cond_init(&barrier->br_busy_cv,NULL);

}

void thread_barrier_wait(th_barrier_t* barrier)
{
    pthread_mutex_lock(&barrier->br_mutex);  // --> Mutex of barrier is locked


    // Scenario: Barrier is in disposition process, all incoming threads wait
    while(barrier->is_ready_again == false)
    {
        pthread_cond_wait(&barrier->br_busy_cv,&barrier->br_mutex);        
    }

    // Scenario: Barrier is still accepting threads and has not started disposition process

    // Last thread to hit
    if(barrier->curr_wait_count +1 == barrier->threshold_count)
    {
        barrier->is_ready_again = false;
        pthread_cond_signal(&barrier->br_cv);
        pthread_mutex_unlock(&barrier->br_mutex);
        return;
    }

    // Threads are incoming, there is space left
    barrier->curr_wait_count += 1;
    pthread_cond_wait(&barrier->br_cv,&barrier->br_mutex);

    // Thread resumes after getting signalled
    barrier->curr_wait_count--;

    // If last thread leaving
    if(barrier->curr_wait_count == 0)
    {
        barrier->is_ready_again = true;
        pthread_cond_broadcast(&barrier->br_busy_cv);
    }
    else
    {
        pthread_cond_signal(&barrier->br_cv);
    }

    pthread_mutex_unlock(&barrier->br_mutex);

}

void thread_barrier_destroy(th_barrier_t* barrier)
{
    barrier->is_ready_again = true;
    pthread_mutex_destroy(&barrier->br_mutex);
    pthread_cond_destroy(&barrier->br_cv);
    pthread_cond_destroy(&barrier->br_busy_cv);
}