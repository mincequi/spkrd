#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/select.h>

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
 * Print number of chars read from stdin
 */
void print_nof_chars(long nof_chars)
{
   /* Set cursor to (row,col) = (5,1) */
   fprintf(stderr, "\033[5;1f");
   fprintf(stderr, "%d", nof_chars);
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

/*
 * Helper function that checks if stdin is ready,
 * call this first to avoid blocking mainloop on 
 * getchar()
 */
int stdin_ready(void) 
{
   fd_set fdset;
   struct timeval timeout;
   FD_ZERO(&fdset);
   FD_SET(0, &fdset);
   timeout.tv_sec = 0;
   timeout.tv_usec = 0;
   return select(1, &fdset, 0, 0, &timeout) > 0;
}

int main(int argc, char *argv[])
{
   struct timespec time;
   time_t seconds = 0;
   long number;
   long nof_stdin_chars = 0;
   
   struct termios term;
   
   if (argc > 1) {
      number = atoi(argv[1]);
   } else {
      number = 0;
   }
 
   /* 
    * Avoid problems with line buffering and interference 
    * from echoed characters
    */
   tcgetattr(0, &term);
   term.c_lflag &= ~(ICANON | ECHO);
   tcsetattr(0, TCSADRAIN, &term);
   
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
      while ( stdin_ready() ) {
	 getchar();
	 nof_stdin_chars++;
	 print_nof_chars(nof_stdin_chars);
      }
   }
   perror("clock_gettime failed");
   return EXIT_FAILURE;	  
}
