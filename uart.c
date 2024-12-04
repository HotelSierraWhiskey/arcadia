#include "uart.h"
#include "sercom.h"
#include "io.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

typedef struct _UART_channel
{
	IO_pin_id_t					rx_pin;
	IO_pin_id_t					tx_pin;
	uint8_t						rx_pad;
	uint8_t						tx_pad;
	SERCOM_channel_t			sercom;
	IO_peripheral_function_t	peripheral_function;
} UART_channel_t;

/****************************************************************************************************
 *	V A R I A B L E S
 ****************************************************************************************************/

static const UART_channel_t uart_channels[UART_CHANNEL_NUM_CHANNELS] =
{
	[UART_CHANNEL_DEBUG] =
	{
		.rx_pin 				= IO_PIN_ID_PA07,
		.tx_pin 				= IO_PIN_ID_PA06,
		.rx_pad 				= 3,
		.tx_pad 				= 2,
		.sercom 				= SERCOM_CHANNEL_0,
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
	UART_channel_t 			channel = uart_channels[channel_id];
	uint8_t 				u8_PCHCTRL_register_index = SERCOM_get_PCHCTRL_register_index(channel.sercom);
	sercom_registers_t * 	p_sercom_registers;

	GCLK_REGS->GCLK_PCHCTRL[u8_PCHCTRL_register_index] = GCLK_PCHCTRL_CHEN(1);

	switch (channel.sercom)
	{
		case SERCOM_CHANNEL_0:
			MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_SERCOM0(1);
			p_sercom_registers = SERCOM0_REGS;
			break;
		case SERCOM_CHANNEL_1:
			MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_SERCOM1(1);
			p_sercom_registers = SERCOM1_REGS;
			break;
		case SERCOM_CHANNEL_2:
			MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_SERCOM2(1);
			p_sercom_registers = SERCOM2_REGS;
			break;
		case SERCOM_CHANNEL_3:
			MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_SERCOM3(1);
			p_sercom_registers = SERCOM3_REGS;
			break;
	}

	IO_enable_peripheral_function_for_pin(channel.rx_pin, channel.peripheral_function);
	IO_enable_peripheral_function_for_pin(channel.tx_pin, channel.peripheral_function);

	p_sercom_registers->USART_INT.SERCOM_BAUD |= 36000;

	p_sercom_registers->USART_INT.SERCOM_CTRLA = SERCOM_USART_INT_CTRLA_MODE_USART_INT_CLK |
												 SERCOM_USART_INT_CTRLA_FORM_USART_FRAME_NO_PARITY |
												 SERCOM_USART_INT_CTRLA_CMODE_ASYNC |
												 SERCOM_USART_INT_CTRLA_DORD_LSB;

	p_sercom_registers->USART_INT.SERCOM_CTRLB = SERCOM_USART_INT_CTRLB_TXEN(1) |
												 SERCOM_USART_INT_CTRLB_RXEN(1) |
												 SERCOM_USART_INT_CTRLB_CHSIZE_8_BIT |
												 SERCOM_USART_INT_CTRLB_SBMODE_1_BIT;

	p_sercom_registers->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_ENABLE(1);

	p_sercom_registers->USART_INT.SERCOM_INTENSET = SERCOM_SPIM_INTENSET_DRE(1) | SERCOM_SPIM_INTENSET_TXC(1);
}
