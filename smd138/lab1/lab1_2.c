#include <stdio.h>
#include <time.h>
#include <stdlib.h>

/*
 * Print time to stderr
 */
void print_time(time_t sec)
{
   /* Set cursor to (row,col) = (1,57) */
   fprintf(stderr, "\033[1;57f");
   fprintf(stderr, "%s", ctime(&sec));
}

/*
 * Print prime number
 */
void print_prime(long number)
{
   /* Set cursor to (row,col) = (3,1) */
   fprintf(stderr, "\033[3;1f");
   fprintf(stderr, "%d", number);
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


int
main(int argc, char *argv[])
{
   struct timespec time;
   time_t seconds = 0;
   int number;
   
   if (argc > 1) {
      number = atoi(argv[1]);
   } else {
      number = 0;
   }
 
   /* Clear screen */
   fprintf(stderr, "\033[2J");
   
   while ( !clock_gettime(CLOCK_REALTIME, &time)) {
      if (time.tv_sec != seconds) {
	 print_time(time.tv_sec);
	 seconds = time.tv_sec;
      }
      if ( is_prime(number) ) {
	 print_prime(number);
      }
      number++;
   }
   perror("clock_gettime failed");
   return EXIT_FAILURE;
	  
}
