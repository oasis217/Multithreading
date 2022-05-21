#ifndef REC_MUTEX_
#define REC_MUTEX_

#include<pthread.h>
#include<stdbool.h>
#include<stdint.h>

typedef struct rec_mutex_ rec_mutex_t;

struct rec_mutex_
{
    // 1) No of self locks by the thread
    uint16_t nlocks;
    // 2) pthread id of the thread which owns this mutex
    pthread_t owner_thread;
    // 3) CV to make other threads blocks when they try to acquire the mutex
    pthread_cond_t cv;
    // 4) Mutex to manipulate this structure in a mutually exclusive way
    pthread_mutex_t state_mutex;
    // 5) No of threads waiting for this mutex lock grant
    uint16_t nwaiting;
};

void rec_mutex_init(rec_mutex_t* rec_mutex);

void rec_mutex_lock(rec_mutex_t* rec_mutex);

void rec_mutex_unlock(rec_mutex_t* rec_mutex);

void rec_mutex_destroy(rec_mutex_t* rec_mutex);

#endif 