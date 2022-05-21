#ifndef RW_LOCK_
#define RW_LOCK_

#include<pthread.h>
#include<stdint.h>
#include<stdbool.h>

typedef struct rw_lock_ rw_lock_t;

struct rw_lock_
{
    //1: A mutex to manipulate, inspect the state of a rw lock in a mutually exclusive way
    pthread_mutex_t state_mutex;

    /*2: A CV to ensure, that when a thread wants to acquire the rw lock, but rw lock is not avaibable,
        we block the fucker*/
    pthread_cond_t cv;

    /*3: Count the no of locks executing inside CS or it could be a case of recusive locking by same thread */
    uint16_t nlocks;

    //4: No of reader thread waiting for the lock grant
    uint16_t n_reader_waiting;

    //5: No of write thread waiting for the lock grant
    uint16_t n_writer_waiting;

    //6: Is lock currently occupied by reader thread
    bool is_locked_by_reader;

    //7. Is lock currently occupied by writer thread
    bool is_locked_by_writer;

    /*8. Thread handle of the write thread currently holding the lock. If it is 0, then the lock is not 
          held by a writer thread */
    pthread_t writer_thread_id; 
};


void rw_lock_init(rw_lock_t* rw_lock);

void rw_lock_rd_lock(rw_lock_t* rw_lock);

void rw_lock_wr_lock(rw_lock_t* rw_lock);

void rw_lock_unlock(rw_lock_t* rw_lock);

void rw_lock_destroy(rw_lock_t* rw_lock);



#endif 