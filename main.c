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
		char c = UART_rx_char(UART_CHANNEL_DEBUG);
		if (c)
		{
			UART_tx_char(UART_CHANNEL_DEBUG, c);
		}
	}
}
