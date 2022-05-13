#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
//-------------------------------------------------
static void* thread1_func(void* arg)
{
    char* input = (char*)arg;

    while(1)
    {
        printf("Input string = %s\n",input);
        sleep(1);
    }
}
//-------------------------------------------------
void panic(char* msg)
{
    printf("%s",msg);
    while(1)
    {}
}
//-------------------------------------------------
void thread1_create(void)
{
    pthread_t pthread1;

    static char* thread_input = "I am thread 1";

    int rc = pthread_create(&pthread1,NULL,thread1_func,(void*)thread_input);

    if(rc != 0)
    {
        panic("Thread creation not succesful\n");
    }
}

//-------------------------------------------------
int main(int argc, char** argv)
{
    thread1_create();
    printf("Main function pausing!\n");
    pause();
    return 0;
}