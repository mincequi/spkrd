/* Beräknar tiden mellan interrupt vid förändring av CTS på COM 1 och COM 2 */
/* Mäter med en snabb klocka */

#include <stdio.h>
#include <stdlib.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/siginfo.h>
#include <time.h>

#include <inttypes.h>
#include <sys/syspage.h>
#include <semaphore.h>

#include "init_comport.h"

const struct sigevent *isr_handler_1(void *arg, int id);
const struct sigevent *isr_handler_2(void *arg, int id);

struct sigevent event_1, event_2;

unsigned char dummy;
uint64_t cycle_1, cycle_2;
sem_t calculate;



void *
int_thread_1(void *arg)
{
    
	fprintf(stderr, "Start thread\n");


	ThreadCtl(_NTO_TCTL_IO, NULL);
	//init_comport(1);

	InterruptAttach(IRQ_4, isr_handler_1, NULL, 0, 0);


	out8(0x3FC, 0x0A); /* Turn on OUT2 and force RTS*/
//	out8(0x2F9, 0x08); /* enable interrupt */

	fprintf(stderr, "Wait for interrupt on COM 1\n");

	while(1)
	{
		InterruptWait(NULL, NULL);

		cycle_1 = ClockCycles( );
		printf("%d cycles elapsed COM 1\n", cycle_1);
	}
}


const struct sigevent *
isr_handler_1(void *arg, int id)
{
	if(in8(MSR + COM_1) & 0x10)
	{
		return (&event_1);
	}
	else
		return (NULL);
}


void *
int_thread_2(void *arg)
{
    
	fprintf(stderr, "Start thread 2\n");


	ThreadCtl(_NTO_TCTL_IO, NULL);
	//init_comport(2);

	InterruptAttach(IRQ_3, isr_handler_2, NULL, 0, 0);

	out8(0x2FC, 0x0A); /* Turn on OUT2 and force RTS*/
//	out8(0x2F9, 0x08); /* enable interrupt */

	fprintf(stderr, "Wait for interrupt on COM 2\n");

	while(1)
	{
		InterruptWait(NULL, NULL);

		cycle_2 = ClockCycles( );
		printf("%d cycles elapsed COM 2\n", cycle_2);

		sem_post(&calculate);
	}
}


const struct sigevent *
isr_handler_2(void *arg, int id)
{
	if(in8(MSR + COM_2) & 0x10)
	{
		return (&event_2);
	}
	else
		return (NULL);
}


main(void)
{
	uint64_t cps, cycle;
	float sec, freq, ncycles;

	fprintf(stderr, "Start main\n");

	sem_init(&calculate, 0, 0);
		
	SIGEV_INTR_INIT(&event_1);
	SIGEV_INTR_INIT(&event_2);

	pthread_create(NULL, NULL, int_thread_1, NULL);
	pthread_create(NULL, NULL, int_thread_2, NULL);
	
	while(1)
	{
		fprintf(stderr, "Main alive\n");
		sem_wait(&calculate);

		ncycles=cycle_2-cycle_1;
		printf("%d cycles elapsed \n", (cycle_2-cycle_1));
		cps = SYSPAGE_ENTRY(qtime)->cycles_per_sec;
		printf( "This system has %lld cycles/sec.\n",cps );
		sec=ncycles/cps;
		printf("The cycles in seconds is %f \n",sec); 

	}
}


#define IRQ_4 	4				/* COM 1 */
#define IRQ_3 	3				/* COM 2 */

#define COM_1 	0x300
#define COM_2 	0x200

#define ICR  		0x20			/* Interrupt Command Register */
#define IMR  	0x21			/* Interrupt Mask Register */

#define THB  	0xF8			/* Transmitter Holding Buffer,			W, 		DLAB=0 */
#define RB   	0xF8			/* Reciver Buffer, 						R, 		DLAB=0 */
#define DLLB 	0xF8			/* Divisor Latch Low Byte,			R/W, 	DLAB=1 */
#define IER  		0xF9			/* Interrupt Enable Register, 			R/W,	DLAB=0 */
#define DLHB 	0xF9			/* Divisor Latch High Byte, 			R/W,	DLAB=1 */
#define IIR  		0xFA			/* Interrupt Identification Register,	R */
#define FCR  	0xFA			/* FIFO Control Register, 				W */
#define LCR  	0xFB			/* Line Control Register, 				R/W */
#define MCR  	0xFC			/* Modem Control Register, 			R/W */
#define LSR  	0xFD			/* Line Status Register, 				R */
#define MSR  	0xFE			/* Modem Status Register, 			R */
#define SR   	0xFF			/* Scratch Register, 					R/W */








