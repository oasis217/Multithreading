/*
 * =====================================================================================
 *
 *       semaphore_hello_world.c ; Allows only 2 threads to enter into CS.
 *
 * =====================================================================================
 */

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>

sem_t sem;
pthread_t thread[5];

#define SEM_PC  2

//----------------------------------------------
void* thread_func(void* arg)
{
    char* thread_name = (char*)arg;

    printf("Thread %s waiting to enter the CS\n",thread_name);

    sem_wait(&sem);

    printf("Thread %s has entered the CS\n",thread_name);

    for(int i=0;i<5;i++)
    {
        printf("Thread %s executing inside the CS\n",thread_name);
        sleep(1);
    }

    printf("Thread %s is exiting the CS\n",thread_name);

    sem_post(&sem);

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
    sem_init(&sem,0,SEM_PC);
    thread_create(&thread[0], "thread0");
    thread_create(&thread[1], "thread1");
    thread_create(&thread[2], "thread2");
    thread_create(&thread[3], "thread3");
    thread_create(&thread[4], "thread4");

    for(int i=0;i<5;i++)
    {
        pthread_join(thread[i],NULL);
    }

    sem_destroy(&sem);
    return 0;


}




