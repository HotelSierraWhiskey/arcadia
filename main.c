#include "sys.h"
#include "io.h"
#include "uart.h"
#include "shell.h"
#include "nvmctrl.h"
#include "chronos.h"
#include "arcadia.h"


/****************************************************************************************************
 *	M A I N
 ****************************************************************************************************/
#include "FreeRTOSConfig.h"

#define xPortSysTickHandler irqSysTick

void vApplicationMallocFailedHook( void ) {
  SHELL_printf("MALLOC FAILED !!\n");
  configASSERT(0);
}

void main( void )
{
	SYS_init();
	SHELL_init();

	// ARCADIA_start();

	// Not reached
    while(1)
	{
		char c = UART_rx_char(UART_CHANNEL_SHELL);

		if (c)
		{
			UART_tx_char(UART_CHANNEL_SHELL, c);
		}
		continue;
	}
}
