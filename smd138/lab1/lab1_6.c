#include <stdio.h>
#include <pthread.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void printout(char *s)
{
   /*pthread_mutex_lock(&mutex);*/
   while (*s)
     putc(*s++, stderr);
   /*pthread_mutex_unlock(&mutex);*/
}

/*
 * Calculate prime numbers
 * 
 * Returns 1 if prime, else 0
 * 
 * Args:
 * number : number to test
 */
int is_prime(long number)
{
   int i;
   
   for (i = 2; i < number; i++) {
      if ( (number % i) == 0 ) {
	 return 0;
      }
   }
   return 1;
}

void *time_thread(void *data)
{
   struct timespec time;
   char buf[32];
   
   while ( !clock_gettime(CLOCK_REALTIME, &time)) {
      sprintf(buf, "\033[1;57f%s", ctime(&time.tv_sec));
      pthread_mutex_lock(&mutex);
      printout(buf);
      pthread_mutex_unlock(&mutex);
      sleep(1);
   }
}

void *prime_thread(void *data)
{
   long *number = (long*)(data);
   char buf[32];
   
   while (1) {
      if ( is_prime(*number) ) {
	 sprintf(buf, "\033[3;1f%d", *number);
	 pthread_mutex_lock(&mutex);
	 printout(buf);
	 pthread_mutex_unlock(&mutex);
      }
      ((long)(*number))++;
   }
}

void *input_thread(void *data)
{
   long nof_inputs = 0;
   char buf[32];
   
   while ( getchar() ) {
      nof_inputs++;
      sprintf(buf, "\033[5;1f%d", nof_inputs);
      pthread_mutex_lock(&mutex);
      printout(buf);
      pthread_mutex_unlock(&mutex);
   }
}

int main(int argc, char *argv[])
{
   struct termios term;
   pthread_t time, prime, input;
   long prime_start;
   char buf[32];
   
   /* 
    * Avoid problems with line buffering and interference 
    * from echoed characters
    */
   
   tcgetattr(0, &term);
   term.c_lflag &= ~(ICANON | ECHO);
   tcsetattr(0, TCSADRAIN, &term);
   
   /* Clear terminal */
   sprintf(buf, "\033[2J");
   printout(buf);
   
   if (argc > 1) {
      prime_start = atoi(argv[1]);
   } else {
      prime_start = 0;
   }
   
   pthread_create(&time, NULL, time_thread, NULL);
   pthread_create(&prime, NULL, prime_thread, &prime_start);
   pthread_create(&input, NULL, input_thread, NULL);
   
   pthread_join(time, NULL);
   
   return 0;
}
