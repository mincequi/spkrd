#include <pthread.h>
#include <stdio.h>


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *thread1(void *data)
{
   printf("Thread 1 started\n");
   while (1) {
      sleep(1);
      
   }
}

int main() 
{
   pthread_create(NULL, NULL, &thread1, NULL);
   
   printf("Main thread\n");
   
   while (1) {
      sleep(5);
   }
   
   return 0;
}
