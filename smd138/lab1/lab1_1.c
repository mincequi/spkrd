#include <stdio.h>
#include <time.h>
#include <stdlib.h>

void print_time(time_t sec)
{
   fprintf(stderr, "\033[2J");
   fprintf(stderr, "\033[1;57f");
   fprintf(stderr, "%s", ctime(&sec));
}

int
main(int argc, char *argv[])
{
   struct timespec time;
   time_t seconds = 0;
   
   while ( !clock_gettime(CLOCK_REALTIME, &time)) {
      if (time.tv_sec != seconds) {
	 print_time(time.tv_sec);
	 seconds = time.tv_sec;
      }
   }
   perror("clock_gettime failed");
   return EXIT_FAILURE;
	  
}
