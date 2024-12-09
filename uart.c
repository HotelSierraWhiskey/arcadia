#include "uart.h"
#include "sercom.h"
#include "io.h"
#include "sys.h"
#include "shell.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define UART_BUFFER_SIZE (1024)

typedef struct _UART_buffer
{
	uint8_t				pu8_data[UART_BUFFER_SIZE];
	volatile uint16_t	u16_head;
	volatile uint16_t	u16_tail;
} UART_buffer_t;

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
	UART_buffer_t						rx_buffer;
	UART_buffer_t						tx_buffer;
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
static UART_channel_t p_uart_channels[UART_CHANNEL_NUM_CHANNELS] =
{
	[UART_CHANNEL_SHELL] =
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
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void 	UART_rx_buffer_push			(UART_channel_id_t channel_id, uint8_t u8_byte);
static uint8_t 	UART_rx_buffer_pop			(UART_channel_id_t channel_id);
static bool 	UART_rx_buffer_is_empty		(UART_channel_id_t channel_id);
static bool 	UART_rx_buffer_is_full		(UART_channel_id_t channel_id);

static void 	UART_tx_buffer_push			(UART_channel_id_t channel_id, uint8_t u8_byte);
static uint8_t 	UART_tx_buffer_pop			(UART_channel_id_t channel_id);
static bool 	UART_tx_buffer_is_empty		(UART_channel_id_t channel_id);
static bool 	UART_tx_buffer_is_full		(UART_channel_id_t channel_id);

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

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

	channel.tx_buffer.u16_head = 0;
	channel.tx_buffer.u16_tail = 0;
	channel.rx_buffer.u16_head = 0;
	channel.rx_buffer.u16_tail = 0;
	memset(channel.rx_buffer.pu8_data, 0, UART_BUFFER_SIZE);
	memset(channel.tx_buffer.pu8_data, 0, UART_BUFFER_SIZE);

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

	channel.p_sercom_registers->USART_INT.SERCOM_INTENSET = SERCOM_USART_INT_INTENSET_TXC(1) |
															SERCOM_USART_INT_INTENSET_RXC(1);
															// SERCOM_USART_INT_INTENSET_DRE(1);
	// temp
	NVIC_EnableIRQ(SERCOM0_IRQn);
}

static void UART_rx_buffer_push(UART_channel_id_t channel_id, uint8_t u8_byte)
{
	UART_buffer_t * buffer = &p_uart_channels[channel_id].rx_buffer;

	if (!UART_rx_buffer_is_full(channel_id))
	{
		buffer->pu8_data[buffer->u16_head] = u8_byte;
		buffer->u16_head = (buffer->u16_head + 1) % UART_BUFFER_SIZE;
	}
}

static uint8_t UART_rx_buffer_pop(UART_channel_id_t channel_id)
{
	UART_buffer_t * buffer = &p_uart_channels[channel_id].rx_buffer;
	uint8_t u8_byte = 0;

	if (!UART_rx_buffer_is_empty(channel_id))
	{
		u8_byte = buffer->pu8_data[buffer->u16_tail];
		buffer->u16_tail = (buffer->u16_tail + 1) % UART_BUFFER_SIZE;
	}
	return u8_byte;
}

static bool UART_rx_buffer_is_empty(UART_channel_id_t channel_id)
{
	UART_buffer_t * buffer = &p_uart_channels[channel_id].rx_buffer;

	return buffer->u16_head == buffer->u16_tail;
}

static bool UART_rx_buffer_is_full(UART_channel_id_t channel_id)
{
	UART_buffer_t * buffer = &p_uart_channels[channel_id].rx_buffer;

	return (buffer->u16_head + 1) % UART_BUFFER_SIZE == buffer->u16_tail;
}

static void UART_tx_buffer_push(UART_channel_id_t channel_id, uint8_t u8_byte)
{
	UART_buffer_t * buffer = &p_uart_channels[channel_id].tx_buffer;

	if (!UART_tx_buffer_is_full(channel_id))
	{
		buffer->pu8_data[buffer->u16_head] = u8_byte;
		buffer->u16_head = (buffer->u16_head + 1) % UART_BUFFER_SIZE;
	}
}

static uint8_t UART_tx_buffer_pop(UART_channel_id_t channel_id)
{
	UART_buffer_t * buffer = &p_uart_channels[channel_id].tx_buffer;
	uint8_t u8_byte = 0;

	if (!UART_tx_buffer_is_empty(channel_id))
	{
		u8_byte = buffer->pu8_data[buffer->u16_tail];
		buffer->u16_tail = (buffer->u16_tail + 1) % UART_BUFFER_SIZE;
	}
	return u8_byte;
}

static bool UART_tx_buffer_is_empty(UART_channel_id_t channel_id)
{
	UART_buffer_t * buffer = &p_uart_channels[channel_id].tx_buffer;

	return buffer->u16_head == buffer->u16_tail;
}

static bool UART_tx_buffer_is_full(UART_channel_id_t channel_id)
{
	UART_buffer_t * buffer = &p_uart_channels[channel_id].tx_buffer;

	return (buffer->u16_head + 1) % UART_BUFFER_SIZE == buffer->u16_tail;
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

	// if (UART_tx_buffer_is_empty(channel_id))
	// {
	// 	p_sercom_registers->USART_INT.SERCOM_INTENSET |= SERCOM_USART_INT_INTENSET_DRE(1);
	// }

	UART_tx_buffer_push(channel_id, (uint8_t)c);
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
	char c = UART_rx_buffer_pop(channel_id);

	return c;
}

void irqSERCOM0()
{
	volatile uint8_t u8_byte;
	
	// This flag is cleared by reading the SERCOM_DATA register
	if ((SERCOM0_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_RXC(1)) != 0)
	{
		IO_set_pin(IO_PIN_ID_PA27, IO_PIN_STATE_HIGH);
		u8_byte = SERCOM0_REGS->USART_INT.SERCOM_DATA;
		// SERCOM0_REGS->USART_INT.SERCOM_INTFLAG = SERCOM_USART_INT_INTFLAG_RXC(1);
		// UART_rx_buffer_push(UART_CHANNEL_SHELL, u8_byte);
		IO_set_pin(IO_PIN_ID_PA27, IO_PIN_STATE_LOW);
	}

	// if ((SERCOM0_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_DRE(1)) != 0)
	// {
	// 	if (!UART_tx_buffer_is_empty(UART_CHANNEL_SHELL))
	// 	{
	// 		u8_byte = UART_tx_buffer_pop(UART_CHANNEL_SHELL);
	// 		SERCOM0_REGS->USART_INT.SERCOM_DATA = u8_byte;
	// 	}
	// 	else
	// 	{
	// 		SERCOM0_REGS->USART_INT.SERCOM_INTENCLR |= SERCOM_USART_INT_INTENCLR_DRE(1);
	// 	}
	// }

	NVIC_ClearPendingIRQ(SERCOM0_IRQn);
}
