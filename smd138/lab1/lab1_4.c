#include <stdio.h>
#include <pthread.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

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
   
   while ( !clock_gettime(CLOCK_REALTIME, &time)) {
      /* Set cursor to (row,col) = (1,57) */
      fprintf(stderr, "\033[1;57f%s", ctime(&time.tv_sec));

      sleep(1);
   }
}

void *prime_thread(void *data)
{
   long *number = (long*)(data);
   
   while (1) {
      if ( is_prime(*number) ) {
	 /* Set cursor to (row,col) = (3,1) */
	 fprintf(stderr, "\033[3;1f%d", *number);
      }
      ((long)(*number))++;
   }
}

void *input_thread(void *data)
{
   long nof_inputs = 0;
   
   while ( getchar() ) {
      nof_inputs++;
      
      /* Set cursor to (row,col) = (5,1) */
      fprintf(stderr, "\033[5;1f%d", nof_inputs);
   }
}

int main(int argc, char *argv[])
{
   struct termios term;
   pthread_t time, prime, input;
   long prime_start;
   
   /* 
    * Avoid problems with line buffering and interference 
    * from echoed characters
    */
   
   tcgetattr(0, &term);
   term.c_lflag &= ~(ICANON | ECHO);
   tcsetattr(0, TCSADRAIN, &term);
   
   /* Clear terminal */
   fprintf(stderr, "\033[2J");
   
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
