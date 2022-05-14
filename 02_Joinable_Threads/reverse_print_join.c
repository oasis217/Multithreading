/* Demonstration of Thread Joining 

Points to remember, pthread_join(pthread_t thread, **void out) : will get the results 
                    returned in *out. Basically the return void* of the thread fuction
                    gets stored in *out.
*/
#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>

//------------------------------------------------------------------------------
void* thread_func_callback(void* arg)
{
    return arg; 

}
//------------------------------------------------------------------------------
void thread_create(pthread_t* thread, int id)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);

    int* _id = malloc(sizeof(int));
    *_id = id;

    int rc = pthread_create(thread,&attr,thread_func_callback,(void*)_id);
    if(rc != 0)
    {
        printf("Thread with input %d could not be created\n",id);
        while(1) {}
    }
}

//------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    int N;

    printf("Please enter the number of threads : ");
    scanf("%d",&N);
    printf("\n");
    printf("You have selected %d threads, Initializing ....\n",N);
    printf("\n");

    if(N <= 0)
    {
        printf("Invalid number of threads, exiting.. \n");
        return 0;
    }

    pthread_t pthread[N];
    void* res = NULL;

    //-----------------------------------
    for(int i=0;i<N;i++)
    {
        thread_create(&pthread[i],i);
    }

    //-----------------------------------
    for(int i=N-1;i>=0;i--)
    {
        pthread_join(pthread[i],&res);
        printf("%d\n",*(int *)res);
        free(res);

    }


    return 0;

}