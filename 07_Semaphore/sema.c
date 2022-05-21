/*
 * =====================================================================================
 *
 *       Sema.c :   This is an implementation of semaphores using mutex and condition 
 *                  variables
 *
 * =====================================================================================
 */

#include "sema.h"
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

//---------------------------------------------------
sema_t* sema_get_new_semaphore()
{
    sema_t* sema=(sema_t*)malloc(sizeof(sema_t));
    return sema;
}
//---------------------------------------------------
void sema_init(sema_t* sema, int count)
{
    sema->permit_counter = count;
    pthread_mutex_init(&sema->mutex,NULL);
    pthread_cond_init(&sema->cond,NULL);
}
//---------------------------------------------------
void sema_wait(sema_t* sema)
{
    pthread_mutex_lock(&sema->mutex);

    sema->permit_counter -= 1;

    if(sema->permit_counter < 0)
    {
        pthread_cond_wait(&sema->cond,&sema->mutex);
    }

    pthread_mutex_unlock(&sema->mutex);

}
//---------------------------------------------------
void sema_post(sema_t* sema)
{
    bool any_thread_waiting;

    pthread_mutex_lock(&sema->mutex);

    any_thread_waiting = sema->permit_counter < 0 ? true:false;
    sema->permit_counter++;

    if(any_thread_waiting)
        pthread_cond_signal(&sema->cond);

    pthread_mutex_unlock(&sema->mutex);

}

//---------------------------------------------------
void sema_destroy(sema_t* sema)
{
    sema->permit_counter = 0;
    pthread_mutex_destroy(&sema->mutex);
    pthread_cond_destroy(&sema->cond);

}

