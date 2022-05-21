#include <assert.h>
#include "rw_locks.h"
//--------------------------------------------------------------------------------------
void rw_lock_init(rw_lock_t* rw_lock)
{
    rw_lock->is_locked_by_reader = false;
    rw_lock->is_locked_by_writer = false;
    rw_lock->n_reader_waiting = 0;
    rw_lock->n_writer_waiting = 0;
    rw_lock->nlocks = 0;
    rw_lock->writer_thread_id = 0;
    pthread_mutex_init(&rw_lock->state_mutex,NULL);
    pthread_cond_init(&rw_lock->cv,NULL);
}
//--------------------------------------------------------------------------------------
void rw_lock_rd_lock(rw_lock_t* rw_lock)
{
    pthread_mutex_lock(&rw_lock->state_mutex);

    //Case-1: Nobody has acquired the rw-lock
    if(rw_lock->is_locked_by_reader == false && rw_lock->is_locked_by_writer == false)
    {
        assert(rw_lock->nlocks == 0);
        assert(!rw_lock->writer_thread_id);

        rw_lock->is_locked_by_reader = true;
        rw_lock->nlocks++;
        pthread_mutex_unlock(&rw_lock->state_mutex);
        return;
    }


    //Case-2: rw-lock is already occupied by another reader thread (or recursive lock by same reader thread) 
    if(rw_lock->is_locked_by_reader == true)
    {
        assert(rw_lock->nlocks);
        assert(rw_lock->is_locked_by_writer == false);
        assert(!rw_lock->writer_thread_id);

        rw_lock->nlocks++;
        pthread_mutex_unlock(&rw_lock->state_mutex);
        return;
    }


    //Case-3: rw_lock has been occupied by some writer thread
    while(rw_lock->is_locked_by_writer == true)
    {
        assert(rw_lock->writer_thread_id);
        assert(rw_lock->nlocks);
        assert(rw_lock->is_locked_by_reader == false);
        
        rw_lock->n_reader_waiting++;
        pthread_cond_wait(&rw_lock->cv,&rw_lock->state_mutex);
        rw_lock->n_reader_waiting--;
    }

    if(rw_lock->nlocks == 0)
    {
        rw_lock->is_locked_by_reader = true;
    }

    rw_lock->nlocks++;
    assert(rw_lock->is_locked_by_writer == false);
    assert(!rw_lock->writer_thread_id);

    pthread_mutex_unlock(&rw_lock->state_mutex);

}
//--------------------------------------------------------------------------------------
void rw_lock_wr_lock(rw_lock_t* rw_lock)
{
    pthread_mutex_lock(&rw_lock->state_mutex);

    //Case-1: Nobody has acquired the rw-lock
    if(rw_lock->is_locked_by_reader == false && rw_lock->is_locked_by_writer == false)
    {
        assert(!rw_lock->nlocks);
        assert(!rw_lock->writer_thread_id);

        rw_lock->nlocks++;
        rw_lock->is_locked_by_writer = true;
        rw_lock->writer_thread_id = pthread_self();
        pthread_mutex_unlock(&rw_lock->state_mutex);
        return;   
    }

    //Case-2: If the same writer thread tries to obtain a recursive lock
    if(rw_lock->is_locked_by_writer == true && rw_lock->writer_thread_id == pthread_self() )
    {
        assert(rw_lock->is_locked_by_reader == false);
        assert(rw_lock->nlocks);
        rw_lock->nlocks++;
        pthread_mutex_unlock(&rw_lock->state_mutex);
        return; 
    }


    //Case-3: If some writer thread tries to obtain a lock on an already locked lock
    while(rw_lock->is_locked_by_reader || rw_lock->is_locked_by_writer)
    {

        /* Sanity check*/
        if(rw_lock->is_locked_by_reader)
        {
            assert(rw_lock->nlocks);
            assert(rw_lock->is_locked_by_writer == false);
            assert(!rw_lock->writer_thread_id);
        }
        else if(rw_lock->is_locked_by_writer)
        {
            assert(rw_lock->nlocks);
            assert(rw_lock->writer_thread_id);
            assert(rw_lock->is_locked_by_reader == false);
        }

        rw_lock->n_writer_waiting++;
        pthread_cond_wait(&rw_lock->cv,&rw_lock->state_mutex);
        rw_lock->n_writer_waiting--;
    }

    assert(rw_lock->is_locked_by_reader == false);
    assert(rw_lock->is_locked_by_writer == false);
    assert(rw_lock->nlocks == 0);
    assert(!rw_lock->writer_thread_id);
    rw_lock->is_locked_by_writer=true;
    rw_lock->writer_thread_id = pthread_self();
    rw_lock->nlocks++;
    pthread_mutex_unlock(&rw_lock->state_mutex);

}
//--------------------------------------------------------------------------------------
void rw_lock_unlock(rw_lock_t* rw_lock)
{
    pthread_mutex_lock(&rw_lock->state_mutex);

    //Case-1 : Attempts to unlock an already unlocked rw_lock
    assert(rw_lock->nlocks);

    //Case-2: Writer thread unlocking the rw_lock
    if(rw_lock->is_locked_by_writer == true && rw_lock->writer_thread_id == pthread_self())
    {
        assert(rw_lock->is_locked_by_reader == false);

        rw_lock->nlocks--;

        if(rw_lock->nlocks)
        {
            // this is recursive unlocking by the same writer thread
            pthread_mutex_unlock(&rw_lock->state_mutex);
            return;
        }
        // this means it is not a recursive unlock, and writer thread is done
        if(rw_lock->n_reader_waiting || rw_lock->n_writer_waiting)
        {
            pthread_cond_broadcast(&rw_lock->cv);
        }

        rw_lock->is_locked_by_writer = false;
        rw_lock->writer_thread_id = 0;
        pthread_mutex_unlock(&rw_lock->state_mutex);
        return;
    }

    //Case-3 Reader thread unlocking the rw-lock
    if(rw_lock->is_locked_by_reader == true)
    {
        assert(rw_lock->is_locked_by_writer == false);
        assert(!rw_lock->writer_thread_id);

        rw_lock->nlocks--;
        if(rw_lock->nlocks)
        {
            // recursive unlock 
            pthread_mutex_unlock(&rw_lock->state_mutex);
            return;
        }

        if(rw_lock->n_reader_waiting || rw_lock->n_writer_waiting)
        {
            pthread_cond_broadcast(&rw_lock->cv);
        }

        rw_lock->is_locked_by_reader = false;
        pthread_mutex_unlock(&rw_lock->state_mutex);
        return;
    }

    assert(0);  // Unreachable state

}
//--------------------------------------------------------------------------------------
void rw_lock_destroy(rw_lock_t* rw_lock)
{
    assert(rw_lock->is_locked_by_reader == false);
    assert(rw_lock->is_locked_by_writer == false);
    assert(rw_lock->n_reader_waiting == 0);
    assert(rw_lock->n_writer_waiting == 0);
    assert(rw_lock->writer_thread_id == 0);
    pthread_mutex_destroy(&rw_lock->state_mutex);
    pthread_cond_destroy(&rw_lock->cv);

}