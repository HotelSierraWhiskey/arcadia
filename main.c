#include "sys.h"
#include "io.h"
#include "uart.h"
#include "shell.h"

/****************************************************************************************************
 *	M A I N
 ****************************************************************************************************/

int main(int argc, char ** argv) 
{
	SYS_init();

	SHELL_init();

	volatile uint32_t i;

	while (1)
	{

		if (UART_rx_char(UART_CHANNEL_SHELL))
		{
			SHELL_printf("\n\r> ");
		}

		// SHELL_printf("Banniversary\n");

		// for (i = 0; i < 4000000; i++)
		// {
		// 	continue;
		// }
	}
}
