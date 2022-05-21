#include <stdio.h>
#include <assert.h>
#include "rec_mutex.h"


void rec_mutex_init(rec_mutex_t* rec_mutex)
{
    rec_mutex->nlocks = 0;
    rec_mutex->owner_thread = 0;
    rec_mutex->nwaiting = 0;
    pthread_mutex_init(&rec_mutex->state_mutex,NULL);
    pthread_cond_init(&rec_mutex->cv,NULL);
}

void rec_mutex_lock(rec_mutex_t* rec_mutex)
{
    pthread_mutex_lock(&rec_mutex->state_mutex);

    //  1) When recursive mutex object is not already locked
    if(rec_mutex->nlocks == 0)
    {
        assert(rec_mutex->owner_thread);
        rec_mutex->owner_thread = pthread_self();
        rec_mutex->nlocks += 1;
        pthread_mutex_unlock(&rec_mutex->state_mutex);
        return;        
    }
    //  2) When recursive mutex is locked, and is being accessed by owner thread
    if(rec_mutex->owner_thread == pthread_self())
    {
        assert(rec_mutex->nlocks);
        rec_mutex->nlocks++;
        pthread_mutex_unlock(&rec_mutex->state_mutex);
        return;
    }

    //  3) When recursive mutex object is locked by some other thread

    while(rec_mutex->owner_thread && rec_mutex->owner_thread != pthread_self())
    {
        rec_mutex->nwaiting++;
        pthread_cond_wait(&rec_mutex->cv,&rec_mutex->state_mutex);
        rec_mutex->nwaiting--;
    } 

    assert(rec_mutex->nlocks == 0);
    assert(rec_mutex->owner_thread == 0);

    rec_mutex->owner_thread = pthread_self();
    rec_mutex->nlocks = 1;
    pthread_mutex_unlock(&rec_mutex->state_mutex);

}

void rec_mutex_unlock(rec_mutex_t* rec_mutex)
{

    pthread_mutex_lock(&rec_mutex->state_mutex);

        //  1) When recursive mutex object is not already locked
        if(rec_mutex->nlocks == 0)
            assert(0);


        //  2) When recursive mutex is locked, and is being accessed by owner thread
        if(rec_mutex->owner_thread == pthread_self())
        {
            assert(rec_mutex->nlocks);  // make sure nlocks>0
            rec_mutex->nlocks--;

            if(rec_mutex->nlocks > 0)
            {
                pthread_mutex_unlock(&rec_mutex->state_mutex);
                return;
            }

            if(rec_mutex->nlocks == 0)
            {
                pthread_cond_signal(&rec_mutex->cv);
                rec_mutex->nlocks = 0;  
                pthread_mutex_unlock(&rec_mutex->state_mutex);
                return; 
            }
            
        }

        //  3) When recursive mutex object is locked by some other thread 
         while(rec_mutex->owner_thread && rec_mutex->owner_thread != pthread_self())
         {
             assert(0);
         }
    
   
}

void rec_mutex_destroy(rec_mutex_t* rec_mutex)
{
    assert(!rec_mutex->nlocks);
    assert(rec_mutex->owner_thread);
    assert(!rec_mutex->nwaiting);
    pthread_mutex_destroy(&rec_mutex->state_mutex);
    pthread_cond_destroy(&rec_mutex->cv);
}
