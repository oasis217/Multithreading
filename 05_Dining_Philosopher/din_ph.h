#ifndef DIN_PHIL_
#define DIN_PHIL_

#include <pthread.h>
#include <stdbool.h>

struct phil_t
{
    int phi_id;
    pthread_t thread_handle;
    int eat_count;
};

struct spoon_t
{
    int spoon_id;
    bool is_used;
    struct phil_t* phil;
    pthread_mutex_t sp_mutex;
    pthread_cond_t sp_cond;
};



#endif