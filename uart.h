#ifndef UART_H
#define UART_H

#include "common.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

typedef enum _UART_baud_rate
{
	UART_BAUD_RATE_ID_9600 = 0,
	UART_BAUD_RATE_ID_19200,
	UART_BAUD_RATE_ID_38400,
	UART_BAUD_RATE_ID_115200,
	//////////
	UART_BAUD_RATE_ID_NUM_BAUD_RATES
} UART_baud_rate_id_t;

typedef enum _UART_channel_id
{
	UART_CHANNEL_SHELL = 0,
	//////////
	UART_CHANNEL_NUM_CHANNELS
} UART_channel_id_t;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void	UART_init		(UART_channel_id_t channel_id);
void	UART_tx_char	(UART_channel_id_t channel_id, char c);
char	UART_rx_char	(UART_channel_id_t channel_id);

#endif // UART_H
