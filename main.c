#include "sys.h"
#include "io.h"
#include "uart.h"

int main(int argc, char ** argv) 
{
	SYS_init();
	UART_init(UART_CHANNEL_DEBUG);
	// UART_init_dbg();

	
	// IO_config_pin_direction(IO_PIN_ID_PA06, IO_DIRECTION_OUTPUT);
	// IO_enable_pullup(IO_PIN_ID_PA06);
	// IO_set_pin(IO_PIN_ID_PA06, IO_PIN_STATE_HIGH);

	while (1)
	{
		// UART_tx_char(UART_CHANNEL_DEBUG, 'P');
		// UART_tx_char(UART_CHANNEL_DEBUG, 'E');
		// UART_tx_char(UART_CHANNEL_DEBUG, 'N');
		// UART_tx_char(UART_CHANNEL_DEBUG, 'I');
		// UART_tx_char(UART_CHANNEL_DEBUG, 'S');
	}
}
