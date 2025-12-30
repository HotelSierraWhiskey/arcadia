#include "uart.h"
#include "printf.h"

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void tfp_uart_putc(void * putp, char c);

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void PRINTF_init(void)
{
	static UART_channel_id_t channel = UART_CHANNEL_SHELL;

	UART_init(channel);
	init_printf(&channel, tfp_uart_putc);
}

static void tfp_uart_putc(void * putp, char c)
{
	UART_channel_id_t channel = *(UART_channel_id_t *)putp;

	if (c == '\n')
	{
		UART_tx_char_raw(channel, '\r');
	}

	UART_tx_char_raw(channel, c);
}
