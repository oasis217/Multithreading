/*
 * =====================================================================================
 *
 *       Zero semaphore, sequential execution example: Print sequential msgs
 *
 * =====================================================================================
 */

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>

sem_t sem[2];
pthread_t pthread[2];

//------------------------------------
void* thread_print_first(void* arg)
{
    char* first_no = (char*)arg;

    while(1)
    {
        printf("%s\n",first_no);
        sleep(2);
        sem_post(&sem[1]);
        sem_wait(&sem[0]);        
    }


}

//------------------------------------
void* thread_print_second(void* arg)
{
    char* second_no = (char*)arg;

    while(1)
    {
        sem_wait(&sem[1]);
        printf("%s\n",second_no);
        sleep(2);
        sem_post(&sem[0]);
    }

}


//-------------------------------------
int main(int argc, char** argv)
{
    sem_init(&sem[0],0,0); 
    sem_init(&sem[1],0,0);

    char* th_msg1 = "a";
    char* th_msg2 = "b";

    int rc = pthread_create(&pthread[0],NULL,thread_print_first,(void*)th_msg1);

    if(rc != 0)
    {
        printf("Thread with msg: %s, could not be created\n",th_msg1);
        return -1;
    }

    rc = pthread_create(&pthread[1],NULL,thread_print_second,(void*)th_msg2);

    if(rc != 0)
    {
        printf("Thread with msg: %s, could not be created\n",th_msg2);
        return -1;
    }

    pthread_exit(0);

    return 0;
}

