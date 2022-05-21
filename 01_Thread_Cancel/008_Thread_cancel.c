#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
//-------------------------------------------------
static void* thread1_func(void* arg)
{
    char* input = (char*)arg;

    int a = 0;

    while(a < 10)
    {
        printf("Input string = %s, Count = %d\n",input,a);
        sleep(1);
        if(a > 5)
        {
            pthread_exit(0);
        }
        a++;


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
    pthread_exit(0);
    return 0;
}
