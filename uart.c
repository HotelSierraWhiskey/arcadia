#include "uart.h"
#include "sercom.h"
#include "io.h"
#include "sys.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

/**
 *	Logical UART channel typedef
 */
typedef struct _UART_channel
{
	IO_pin_id_t							rx_pin;
	IO_pin_id_t							tx_pin;
	uint32_t							u32_rx_pad;
	uint32_t							u32_tx_pad;
	UART_baud_rate_id_t					baud_rate;
	SERCOM_channel_id_t					sercom_channel_id;
	volatile sercom_registers_t *		p_sercom_registers;
	IO_peripheral_function_t			peripheral_function;
} UART_channel_t;

/****************************************************************************************************
 *	V A R I A B L E S
 ****************************************************************************************************/

/**
 *	Canned values to write in the SERCOM's BAUD register
 *
 */
static const uint32_t kpu32_pre_calculated_baud_register_values[UART_BAUD_RATE_ID_NUM_BAUD_RATES] =
{
	[UART_BAUD_RATE_ID_9600] 	= 65326UL,
	[UART_BAUD_RATE_ID_19200] 	= 65116UL,
	[UART_BAUD_RATE_ID_38400] 	= 64697UL,
	[UART_BAUD_RATE_ID_115200] 	= 63019UL
};

/**
 *	UART channels
 *
 * 	A list of all configured logical UART channels 
 */
static const UART_channel_t p_uart_channels[UART_CHANNEL_NUM_CHANNELS] =
{
	[UART_CHANNEL_DEBUG] =
	{
		.rx_pin 				= IO_PIN_ID_PA07,
		.tx_pin 				= IO_PIN_ID_PA06,
		.u32_rx_pad 			= SERCOM_USART_INT_CTRLA_RXPO_PAD3,
		.u32_tx_pad 			= SERCOM_USART_INT_CTRLA_TXPO_PAD1,
		.baud_rate				= UART_BAUD_RATE_ID_115200,
		.sercom_channel_id 		= SERCOM_CHANNEL_ID_0,
		.p_sercom_registers		= SERCOM0_REGS,
		.peripheral_function 	= IO_PERIPHERAL_FUNCTION_D
	}
};

/****************************************************************************************************
 *	UART Initialization
 *
 * 	@param[in] channel_id The logical channel to initialize
 * 
 * 	@note For now, this initialization sequence supports internal clock mode only
 *
 ****************************************************************************************************/
void UART_init(UART_channel_id_t channel_id)
{
	UART_channel_t 			channel = p_uart_channels[channel_id];
	uint8_t 				u8_PCHCTRL_register_index = SERCOM_get_PCHCTRL_register_index(channel.sercom_channel_id);

	GCLK_REGS->GCLK_PCHCTRL[u8_PCHCTRL_register_index] = 	GCLK_PCHCTRL_CHEN(1) | 
															GCLK_PCHCTRL_GEN_GCLK0;

	switch (channel.sercom_channel_id)
	{
		case SERCOM_CHANNEL_ID_0:
			MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_SERCOM0(1);
			break;
		case SERCOM_CHANNEL_ID_1:
			MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_SERCOM1(1);
			break;
		case SERCOM_CHANNEL_ID_2:
			MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_SERCOM2(1);
			break;
		case SERCOM_CHANNEL_ID_3:
			MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_SERCOM3(1);
			break;
	}

	while ((MCLK_REGS->MCLK_INTFLAG & MCLK_INTFLAG_CKRDY(1)) == 0)
	{
		continue;
	}

	IO_enable_peripheral_function_for_pin(channel.rx_pin, channel.peripheral_function);
	IO_enable_peripheral_function_for_pin(channel.tx_pin, channel.peripheral_function);

	channel.p_sercom_registers->USART_INT.SERCOM_CTRLA = 	channel.u32_rx_pad | 
															channel.u32_tx_pad;

	channel.p_sercom_registers->USART_INT.SERCOM_BAUD = kpu32_pre_calculated_baud_register_values[channel.baud_rate];

	channel.p_sercom_registers->USART_INT.SERCOM_CTRLA |=	SERCOM_USART_INT_CTRLA_MODE_USART_INT_CLK |
															SERCOM_USART_INT_CTRLA_FORM_USART_FRAME_NO_PARITY |
															SERCOM_USART_INT_CTRLA_CMODE_ASYNC |
															SERCOM_USART_INT_CTRLA_DORD_LSB;

	channel.p_sercom_registers->USART_INT.SERCOM_CTRLB =	SERCOM_USART_INT_CTRLB_TXEN(1) |
												 			SERCOM_USART_INT_CTRLB_RXEN(1) |
												 			SERCOM_USART_INT_CTRLB_CHSIZE_8_BIT |
												 			SERCOM_USART_INT_CTRLB_SBMODE_1_BIT;

	channel.p_sercom_registers->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_ENABLE(1);

	// Wait to syncronize after enabling
	while (channel.p_sercom_registers->USART_INT.SERCOM_SYNCBUSY & SERCOM_USART_INT_SYNCBUSY_ENABLE(1))
	{
		continue;
	}

	channel.p_sercom_registers->USART_INT.SERCOM_INTENSET = SERCOM_USART_INT_INTENSET_DRE(1) |
															SERCOM_USART_INT_INTENSET_TXC(1) |
															SERCOM_USART_INT_INTENSET_RXC(1);
}

/****************************************************************************************************
 *	Sends a character over the selected UART interface
 *
 * 	@param[in] channel_id The logical channel to initialize
 * 	@param[in] c The char to send
 *
 ****************************************************************************************************/
void UART_tx_char(UART_channel_id_t channel_id, char c)
{
	volatile sercom_registers_t * p_sercom_registers = p_uart_channels[channel_id].p_sercom_registers;

	p_sercom_registers->USART_INT.SERCOM_DATA = c;

	while ((p_sercom_registers->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_TXC(1)) == 0)
	{
		continue;
	}
}

/****************************************************************************************************
 *	Receives a character over the selected UART interface
 *
 * 	@param[in] channel_id The logical channel to initialize
 * 
 * 	@return the received character if one was retrieved from the DATA register, else 0
 *
 ****************************************************************************************************/
char UART_rx_char(UART_channel_id_t channel_id)
{
	volatile sercom_registers_t * p_sercom_registers = p_uart_channels[channel_id].p_sercom_registers;

	if ((p_sercom_registers->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_RXC(1)) != 0)
	{
		return p_sercom_registers->USART_INT.SERCOM_DATA;
	}

	return 0;
}
