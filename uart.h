#ifndef UART_H
#define UART_H

#include "common.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

typedef enum _UART_channel_id
{
	UART_CHANNEL_DEBUG = 0,
	//////////
	UART_CHANNEL_NUM_CHANNELS
} UART_channel_id_t;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/
void UART_init_dbg(void);
void	UART_init		(UART_channel_id_t channel_id);
void	UART_tx_char	(UART_channel_id_t channel_id, char c);
char	UART_rx_char	(UART_channel_id_t channel_id);

#endif // UART_H
