/* Demonstration of Thread Joining 

Points to remember, pthread_join(pthread_t thread, **void out) : will get the results 
                    returned in *out. Basically the return void* of the thread fuction
                    gets stored in *out.
*/
#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>

void* thread_func_callback(void* arg)
{
    int input_no = *(int*)arg;
    int rc = 0;

    free(arg);

    while(rc != input_no)
    {   
        printf(" Thread with input %d doig some work\n",input_no);
        sleep(1);
        rc++;
    }

    int* _res = malloc(sizeof(int));
    *_res = input_no * input_no;;

    return (void*)_res; 

}

void thread_create(pthread_t* thread, int input)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);

    int* _input = malloc(sizeof(int));
    *_input = input;

    int rc = pthread_create(thread,&attr,thread_func_callback,(void*)_input);
    if(rc != 0)
    {
        printf("Thread with input %d could not be created\n",input);
        while(1) {}
    }
}



pthread_t pthread2;
pthread_t pthread3;

int main(int argc, char** argv)
{
    void* pthread2_result = NULL;
    void* pthread3_result = NULL;

    thread_create(&pthread2,2);
    thread_create(&pthread3,10);

    printf("Main function is blocked until thread 2 return\n");
    pthread_join(pthread2,&pthread2_result);

    if(pthread2_result)
    {
        printf(" Return result from thread 2 = %d\n",*(int *)pthread2_result);
        free(pthread2_result);
        pthread2_result = NULL;
    }

    printf("Main function is blocked until thread 3 return\n");
    pthread_join(pthread3,&pthread3_result);

    if(pthread3_result)
    {
        printf(" Return result from thread 3 = %d\n",*(int *)pthread3_result);
        free(pthread3_result);
        pthread3_result = NULL;
    }

    return 0;

}