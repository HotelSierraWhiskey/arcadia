#include "sys.h"
#include "io.h"
#include "uart.h"

int main(int argc, char ** argv) 
{
	SYS_init();
	UART_init(UART_CHANNEL_DEBUG);
	volatile uint32_t i;
	while (1)
	{
		UART_tx_char(UART_CHANNEL_DEBUG, 'P');
		UART_tx_char(UART_CHANNEL_DEBUG, 'E');
		UART_tx_char(UART_CHANNEL_DEBUG, 'N');
		UART_tx_char(UART_CHANNEL_DEBUG, 'I');
		UART_tx_char(UART_CHANNEL_DEBUG, 'S');

		for (i = 0; i < 1000000; i++)
		{

		}
	}
}
