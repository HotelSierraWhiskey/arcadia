#include "sys.h"
#include "mempool.h"
#include "arcadia.h"

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	Program entry
 * 
 * 	Initializes system drivers, pre-allocates shared buffers, and starts the scheduler
 * 
 ****************************************************************************************************/
void main(void)
{
	SYS_init();

	MEMPOOL_init();

	#include "timer.h"
	#warning "remove me"
	TIMER_start_dma_timer();

	ARCADIA_start();
}
