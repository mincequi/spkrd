#include <pthread.h>
#include <stdio.h>
#include <sys/neutrino.h>
#include <time.h>

#define REPLY_BUFFER_SIZE 10
#define RECV_BUFFER_SIZE  10

int chid;

void *thread1(void *data)
{
   struct _pulse pulse;
   struct timespec t;
   void *ptr;
   int recvid;
   void *addr;
   
   printf("Thread 1 started\n");
   
   while (1) {
      recvid = MsgReceivePulse(chid, &pulse, sizeof(pulse), NULL);
      printf("Thread 1, message received: %#x\n", pulse.value.sival_ptr, recvid);
      
      t = *((struct timespec *)(pulse.value.sival_int));
      printf("Unix time: %d\n", t.tv_sec);
      
      sleep(1);
   }
}

int main() 
{
   int conid, result;
   int value = 42;
   struct timespec now;
   
   chid = ChannelCreate(0);
   
   
   pthread_create(NULL, NULL, &thread1, NULL);
   
   
   conid = ConnectAttach(0, 0, chid, 0, 0); 
   
   printf("Main thread started\n");
      
   
   while (1) {
      clock_gettime(CLOCK_REALTIME, &now);
      result = MsgSendPulse(conid, 1, 0, (int)(&now));
      printf("Main thread, MsgPulseSend  result: %d\n", result);
      sleep(5);
   }
   
   return 0;
}
