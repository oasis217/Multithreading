#ifndef SEMA_H_
#define SEMA_H_

#include <pthread.h>

typedef struct sema_ sema_t;

struct sema_
{
    int permit_counter;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

sema_t* sema_get_new_semaphore();

void sema_init(sema_t* sema, int count);

void sema_post(sema_t* sema);

void sema_wait(sema_t* sema);

void sema_destroy(sema_t* sema);


#endif