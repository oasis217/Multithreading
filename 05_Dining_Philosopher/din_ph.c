/*
Problem Statement :
====================

1)  Philosopher can eat when he has access to both spoons.

2)  Philosopher will try to access left spoon first, and then right spoon

3)  If after getting access to the left spoon, right spoon is not available
    the philosopher will release the left spoon and restart from beginning after
    a wait of 1s.

4)  Philosopher enjoys the cake for 1s, after he has access to both spoons.

5)  Philosopher makes an attempt of 2nd slice after a wait of 1s.

6)  All philosopher run in an infinite loop.

7)  Non-adjacent philosophers can eat in parallel.

*/

#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<unistd.h>
#include"din_ph.h"

//----------------------------------------------

#define N_PHILOSOPHERS  5

static struct spoon_t spoon[N_PHILOSOPHERS];
static struct phil_t   philosopher[N_PHILOSOPHERS];

//----------------------------------------------

static struct spoon_t* phil_get_left_spoon(struct phil_t* phil)
{
    return &spoon[phil->phi_id];
}

//-----------------------------------------------

static struct spoon_t* phil_get_right_spoon(struct phil_t* phil)
{
    if(phil->phi_id == 0)
        return &spoon[N_PHILOSOPHERS-1];

    return &spoon[phil->phi_id -1];
}

//-----------------------------------------------
void phil_eat(struct phil_t* phil)
{
    
    int phil_id = phil->phi_id;
    struct spoon_t* left_spoon = phil_get_left_spoon(phil);
    struct spoon_t* right_spoon = phil_get_right_spoon(phil);

    //********************** Check conditions
    printf("Phil %d starting to eat with spoon [%d, %d] for %d times\n",phil->phi_id,left_spoon->spoon_id,right_spoon->spoon_id,phil->eat_count);
    assert(left_spoon->phil == phil);
    assert(right_spoon->phil == phil);
    assert(left_spoon->is_used == true);
    assert(right_spoon->is_used == true);
    phil->eat_count++;
    printf("Phil %d eats with spoon [%d, %d] for %d times\n",phil->phi_id,left_spoon->spoon_id,right_spoon->spoon_id,phil->eat_count);
    //printf("\n");
    sleep(1);
}

//-----------------------------------------------
void philosopher_release_both_spoons(struct phil_t *phil) 
{

	struct spoon_t *left_spoon  = phil_get_left_spoon(phil);
	struct spoon_t *right_spoon = phil_get_right_spoon(phil);

    left_spoon->is_used = false;
    right_spoon->is_used = false;
    left_spoon->phil = NULL;
    right_spoon->phil = NULL;

    pthread_cond_signal(&left_spoon->sp_cond);
    pthread_cond_signal(&right_spoon->sp_cond);

    printf("Phil %d is releasing left (%d) and right (%d) spoon, and going to sleep\n",phil->phi_id,left_spoon->spoon_id,right_spoon->spoon_id);
    //printf("\n");

    pthread_mutex_unlock(&right_spoon->sp_mutex);
    pthread_mutex_unlock(&left_spoon->sp_mutex);
    

	/* . . .  */
}
//------------------------------------------------
bool philosopher_get_access_both_spoons(struct phil_t *phil)
{

	struct spoon_t *left_spoon  = phil_get_left_spoon(phil);
	struct spoon_t *right_spoon = phil_get_right_spoon(phil);

    pthread_mutex_lock(&left_spoon->sp_mutex);
    left_spoon->is_used = true;
    left_spoon->phil = phil;
    printf("Phil %d got access to left spoon (%d)\n",phil->phi_id,left_spoon->spoon_id);


    while(right_spoon->is_used == true)
    {        
        printf("Phil %d could not access the right spoon (%d), going to sleep and giving up (%d) spoon...\n",phil->phi_id,right_spoon->spoon_id,left_spoon->spoon_id);
        //printf("\n");    
        pthread_cond_signal(&left_spoon->sp_cond);
        left_spoon->is_used = false;
        left_spoon->phil = NULL;
        pthread_cond_wait(&right_spoon->sp_cond,&left_spoon->sp_mutex);
        left_spoon->is_used = true;
        left_spoon->phil = phil;
    }

    pthread_mutex_lock(&right_spoon->sp_mutex);
    right_spoon->is_used = true;
    right_spoon->phil = phil;
    printf("Phil %d got access to right spoon (%d)\n",phil->phi_id,right_spoon->spoon_id);
    //printf("\n");


	/* . . .  */
	return true;
}


//-----------------------------------------------
void* phil_func(void* phil_arg)
{
    struct phil_t* phil  = (struct phil_t*)phil_arg;
    while(1)
    {
        if(philosopher_get_access_both_spoons(phil))
        {
            phil_eat(phil);
            philosopher_release_both_spoons(phil);
            sleep(1);
        }              
    }
}

//-----------------------------------------------
int main(int argc, char** argv)
{
    int i=0;
    pthread_attr_t attr;

    //----------------------------------------------------
    for(i=0;i<N_PHILOSOPHERS;i++)
    {
        spoon[i].is_used = false;
        spoon[i].phil = NULL;
        spoon[i].spoon_id = i;
        pthread_mutex_init(&spoon[i].sp_mutex,NULL);
        pthread_cond_init(&spoon[i].sp_cond,NULL);
    }    
    //----------------------------------------------------

    /*  Default attributes of all Philosopher threads */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);


    //----------------------------------------------------
    for(i=0;i<N_PHILOSOPHERS;i++)
    {
        philosopher[i].phi_id = i;
        philosopher[i].eat_count = 0;
        pthread_create(&philosopher[i].thread_handle,&attr,phil_func,(void*)&philosopher[i]);
    }

    pthread_exit(0);

    return 0;
}
