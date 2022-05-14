/*
In this example we have two threads T_sum and T_swap. 

T_sum sums up the array  ar = {1,2,3,4,5} so the sum should be 16

T_swap swaps the first and last value of the array ar.

Compile the program :

gcc -g -c Sum_Swap_with_mutex.c -o Sum_Swap_with_mutex.o
gcc -g Sum_Swap_with_mutex.o -o Sum_Swap_with_mutex.out -lpthread

Check output

*/



#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>

int arr[] = {1,2,3,4,5};
pthread_mutex_t arr_mutex;

//-------------------------------------------
static void* func_sum(void* arg)
{
    int size = sizeof(arr)/sizeof(arr[0]);
    
    while(1)
    {
        pthread_mutex_lock(&arr_mutex);
        int sum = 0;
        for(int i=0; i<size; i++)
        {
            sum = sum + arr[i];
        }

        printf("Sum of array : %d\n",sum);
        pthread_mutex_unlock(&arr_mutex);
    }
}

//-----------------------------------------------
int pthread_sum_create()
{
    pthread_t pthread_sum;
    int rc = 0;

    rc = pthread_create(&pthread_sum,NULL,func_sum,NULL);

    if(rc != 0)
    {
        printf("Failed to created sum thread\n");
        return rc;
    }

}


//-----------------------------------------------
static void* func_swap(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&arr_mutex);
        int temp;
        int size = sizeof(arr)/sizeof(arr[0]);
        temp = arr[0];
        arr[0]  = arr[size-1];
        arr[size-1] = temp;
        pthread_mutex_unlock(&arr_mutex);
    }

}
//-----------------------------------------------
int pthread_swap_create()
{
    pthread_t pthread_swap;
    int rc = 0;

    rc = pthread_create(&pthread_swap,NULL,func_swap,NULL);

    if(rc != 0)
    {
        printf("Failed to created swap thread\n");
        return rc;
    }

}

//-------------------------------------------
int main(int argc, char** argv)
{
    pthread_mutex_init(&arr_mutex,NULL);
    pthread_sum_create();
    pthread_swap_create();

    pthread_exit(0);

}