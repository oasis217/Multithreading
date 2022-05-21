/*
 * =====================================================================================
 *
 *       semaaphore_hello_world.c ; Allows only 2 threads to enter into CS.
 *
 * =====================================================================================
 */

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include "sema.h"

sema_t sema;
pthread_t thread[5];

#define sema_PC  2

//----------------------------------------------
void* thread_func(void* arg)
{
    char* thread_name = (char*)arg;

    printf("Thread %s waiting to enter the CS\n",thread_name);

    sema_wait(&sema);

    printf("Thread %s has entered the CS\n",thread_name);

    for(int i=0;i<5;i++)
    {
        printf("Thread %s executing inside the CS\n",thread_name);
        sleep(1);
    }

    printf("Thread %s is exiting the CS\n",thread_name);

    sema_post(&sema);

    printf("Thread %s has exited the CS\n",thread_name);


}


//----------------------------------------------
void thread_create(pthread_t* pthread, void* arg)
{
    int rc = pthread_create(pthread,NULL,thread_func,arg);

    if(rc != 0)
    {
        printf("Thread %s could not be created!\n",(char*)arg);
        while(1) {}
    }
}


//----------------------------------------------
int main(int argc, char** argv)
{
    sema_init(&sema,sema_PC);
    thread_create(&thread[0], "thread0");
    thread_create(&thread[1], "thread1");
    thread_create(&thread[2], "thread2");
    thread_create(&thread[3], "thread3");
    thread_create(&thread[4], "thread4");

    for(int i=0;i<5;i++)
    {
        pthread_join(thread[i],NULL);
    }

    sema_destroy(&sema);
    return 0;


}




