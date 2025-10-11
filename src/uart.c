#include "uart.h"
#include "sercom.h"
#include "io.h"
#include "sys.h"
#include "shell.h"
#include "common.h"
#include "arcadia.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define UART_BUFFER_SIZE (512U)

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
	// Must be populated
	const char *					kpc_name;
	ARCADIA_task_id_t				owner;
	IO_pin_id_t						rx_pin;
	IO_pin_id_t						tx_pin;
	uint32_t						u32_rx_pad;
	uint32_t						u32_tx_pad;
	UART_baud_rate_id_t				baud_rate_id;
	UART_buffer_t					rx_buffer;
	UART_buffer_t					tx_buffer;
	SERCOM_channel_id_t				sercom_channel_id;
	IO_peripheral_function_t		peripheral_function;

	// Populated during initialization (derived from `sercom_channel_id`)
	IRQn_Type						_irq_index;
	sercom_usart_int_registers_t *	_p_sercom_registers;
} UART_channel_t;

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

/**
 *	Actual baud rate value constants
 */
static const uint32_t kpu8_baud_rates[UART_BAUD_RATE_ID_NUM_BAUD_RATES] =
{
	[UART_BAUD_RATE_ID_9600] 	= 9600,
	[UART_BAUD_RATE_ID_19200] 	= 19200,
	[UART_BAUD_RATE_ID_38400] 	= 38400,
	[UART_BAUD_RATE_ID_115200] 	= 115200
};

/**
 * Registry of logical UART channels
 */
static UART_channel_t p_uart_channels[UART_CHANNEL_NUM_CHANNELS] =
{
#ifdef AUDIO_SWITCH
	[UART_CHANNEL_SHELL] =
	{
		.kpc_name				= "Debug Shell",
		.owner					= ARCADIA_TASK_ID_SHELL,
		.rx_pin 				= IO_PIN_ID_PA25,
		.tx_pin 				= IO_PIN_ID_PA24,
		.u32_rx_pad 			= SERCOM_USART_INT_CTRLA_RXPO_PAD3,
		.u32_tx_pad 			= SERCOM_USART_INT_CTRLA_TXPO_PAD1,
		.baud_rate_id			= UART_BAUD_RATE_ID_115200,
		.sercom_channel_id 		= SERCOM_CHANNEL_ID_3,
		.peripheral_function 	= IO_PERIPHERAL_FUNCTION_C
	},
#endif // AUDIO_SWITCH

#ifdef DEV_BOARD
	[UART_CHANNEL_SHELL] =
	{
		.kpc_name				= "Debug Shell",
		.owner					= ARCADIA_TASK_ID_SHELL,
		.rx_pin 				= IO_PIN_ID_PA07,
		.tx_pin 				= IO_PIN_ID_PA06,
		.u32_rx_pad 			= SERCOM_USART_INT_CTRLA_RXPO_PAD3,
		.u32_tx_pad 			= SERCOM_USART_INT_CTRLA_TXPO_PAD1,
		.baud_rate_id			= UART_BAUD_RATE_ID_115200,
		.sercom_channel_id 		= SERCOM_CHANNEL_ID_0,
		.peripheral_function 	= IO_PERIPHERAL_FUNCTION_D
	},
#endif // DEV_BOARD
};

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void			UART_buffers_init			(UART_channel_id_t channel_id);

static void 		UART_rx_buffer_push			(UART_channel_id_t channel_id, uint8_t u8_byte);
static uint8_t 		UART_rx_buffer_pop			(UART_channel_id_t channel_id);
static bool 		UART_rx_buffer_is_empty		(UART_channel_id_t channel_id);
static bool 		UART_rx_buffer_is_full		(UART_channel_id_t channel_id);

static void 		UART_tx_buffer_push			(UART_channel_id_t channel_id, uint8_t u8_byte);
static uint8_t 		UART_tx_buffer_pop			(UART_channel_id_t channel_id);
static bool 		UART_tx_buffer_is_empty		(UART_channel_id_t channel_id);
static bool 		UART_tx_buffer_is_full		(UART_channel_id_t channel_id);

static uint32_t		UART_calculate_baud_value	(UART_baud_rate_id_t baud_id);

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	UART channel initialization
 *
 * 	@param[in] channel_id The logical UART channel to initialize
 * 
 * 	@note For now, this initialization sequence supports internal clock mode only
 *
 ****************************************************************************************************/
void UART_init(UART_channel_id_t channel_id)
{
	UART_channel_t 	* 	p_channel = &p_uart_channels[channel_id];
	uint8_t 			u8_PCHCTRL_register_index = SERCOM_get_PCHCTRL_register_index(p_channel->sercom_channel_id);

	UART_buffers_init(channel_id);

	GCLK_REGS->GCLK_PCHCTRL[u8_PCHCTRL_register_index] = 	GCLK_PCHCTRL_CHEN(1) | 
															GCLK_PCHCTRL_GEN_GCLK0;

	switch (p_channel->sercom_channel_id)
	{
		case SERCOM_CHANNEL_ID_0:
			MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_SERCOM0(1);
			p_channel->_irq_index = SERCOM0_IRQn;
			p_channel->_p_sercom_registers = &SERCOM0_REGS->USART_INT;
			break;
		case SERCOM_CHANNEL_ID_1:
			MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_SERCOM1(1);
			p_channel->_irq_index = SERCOM1_IRQn;
			p_channel->_p_sercom_registers = &SERCOM1_REGS->USART_INT;
			break;
		case SERCOM_CHANNEL_ID_2:
			MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_SERCOM2(1);
			p_channel->_irq_index = SERCOM2_IRQn;
			p_channel->_p_sercom_registers = &SERCOM2_REGS->USART_INT;
			break;
		case SERCOM_CHANNEL_ID_3:
			MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_SERCOM3(1);
			p_channel->_irq_index = SERCOM3_IRQn;
			p_channel->_p_sercom_registers = &SERCOM3_REGS->USART_INT;
			break;
	}

	while ((MCLK_REGS->MCLK_INTFLAG & MCLK_INTFLAG_CKRDY(1)) == 0)
	{
		continue;
	}

	IO_enable_peripheral_function_for_pin(p_channel->rx_pin, p_channel->peripheral_function);
	IO_enable_peripheral_function_for_pin(p_channel->tx_pin, p_channel->peripheral_function);

	p_channel->_p_sercom_registers->SERCOM_CTRLA = 	p_channel->u32_rx_pad | 
																p_channel->u32_tx_pad;

	p_channel->_p_sercom_registers->SERCOM_BAUD = UART_calculate_baud_value(p_channel->baud_rate_id);

	p_channel->_p_sercom_registers->SERCOM_CTRLA |=	SERCOM_USART_INT_CTRLA_MODE_USART_INT_CLK |
													SERCOM_USART_INT_CTRLA_FORM_USART_FRAME_NO_PARITY |
													SERCOM_USART_INT_CTRLA_CMODE_ASYNC |
													SERCOM_USART_INT_CTRLA_DORD_LSB;

	p_channel->_p_sercom_registers->SERCOM_CTRLB =	SERCOM_USART_INT_CTRLB_TXEN(1) |
												 	SERCOM_USART_INT_CTRLB_RXEN(1) |
												 	SERCOM_USART_INT_CTRLB_CHSIZE_8_BIT |
												 	SERCOM_USART_INT_CTRLB_SBMODE_1_BIT;

	p_channel->_p_sercom_registers->SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_ENABLE(1);

	// Wait to syncronize after enabling
	while (p_channel->_p_sercom_registers->SERCOM_SYNCBUSY & SERCOM_USART_INT_SYNCBUSY_ENABLE(1))
	{
		continue;
	}

	p_channel->_p_sercom_registers->SERCOM_INTENSET = SERCOM_USART_INT_INTENSET_RXC(1);
	
	NVIC_EnableIRQ(p_channel->_irq_index);
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
	sercom_usart_int_registers_t * p_sercom_registers = p_uart_channels[channel_id]._p_sercom_registers;

	UART_tx_buffer_push(channel_id, (uint8_t)c);

	p_sercom_registers->SERCOM_INTENSET |= SERCOM_USART_INT_INTENSET_DRE(1);
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

/****************************************************************************************************
 *	Zeroes a channel's UART buffers
 *
 * 	@param[in] channel_id The logical channel whose buffers are to be initialized
 *
 ****************************************************************************************************/
static void UART_buffers_init(UART_channel_id_t channel_id)
{
	UART_channel_t * p_channel = &p_uart_channels[channel_id];

	p_channel->tx_buffer.u16_head = 0;
	p_channel->tx_buffer.u16_tail = 0;
	p_channel->rx_buffer.u16_head = 0;
	p_channel->rx_buffer.u16_tail = 0;
	memset(p_channel->rx_buffer.pu8_data, 0, UART_BUFFER_SIZE);
	memset(p_channel->tx_buffer.pu8_data, 0, UART_BUFFER_SIZE);
}

/****************************************************************************************************
 *	Pushes a byte onto the RX buffer
 *
 * 	@param[in] channel_id The desired UART channel
 *	@param[in] u8_byte A byte of UART data
 *
 *	@note
 *	Called in ISR context
 *
 ****************************************************************************************************/
static void UART_rx_buffer_push(UART_channel_id_t channel_id, uint8_t u8_byte)
{
	UART_buffer_t * buffer = &p_uart_channels[channel_id].rx_buffer;

	if (!UART_rx_buffer_is_full(channel_id))
	{
		buffer->pu8_data[buffer->u16_head] = u8_byte;
		buffer->u16_head = (buffer->u16_head + 1) % UART_BUFFER_SIZE;
	}
}

/****************************************************************************************************
 *	Pops a byte off of a given UART's RX buffer, and adjusts the tail index
 *
 * 	@param[in] channel_id The desired UART channel
 *
 *	@note
 *	Called in ISR context
 *
 * 	@return A byte off of the UART's RX buffer
 * 
 ****************************************************************************************************/
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

/****************************************************************************************************
 *	Checks whether a given UART's RX buffer is empty
 *
 * 	@param[in] channel_id The desired UART channel
 *
 *	@note
 *	Called in ISR context
 *
 *	@return `true` if the buffer is empty, else `false`
 ****************************************************************************************************/
static bool UART_rx_buffer_is_empty(UART_channel_id_t channel_id)
{
	UART_buffer_t * buffer = &p_uart_channels[channel_id].rx_buffer;

	return buffer->u16_head == buffer->u16_tail;
}

/****************************************************************************************************
 *	Checks whether a given UART's RX buffer is full
 *
 * 	@param[in] channel_id The desired UART channel
 *
 *	@note
 *	Called in ISR context
 *
 *	@return `true` if the buffer is full, else `false`
 ****************************************************************************************************/
static bool UART_rx_buffer_is_full(UART_channel_id_t channel_id)
{
	UART_buffer_t * buffer = &p_uart_channels[channel_id].rx_buffer;

	return (buffer->u16_head + 1) % UART_BUFFER_SIZE == buffer->u16_tail;
}

/****************************************************************************************************
 *	Pushes a byte onto the TX buffer
 *
 * 	@param[in] channel_id The desired UART channel
 *	@param[in] u8_byte A byte of UART data
 *
 *	@note
 *	Called in ISR context
 *
 ****************************************************************************************************/
static void UART_tx_buffer_push(UART_channel_id_t channel_id, uint8_t u8_byte)
{
	UART_buffer_t * buffer = &p_uart_channels[channel_id].tx_buffer;

	if (!UART_tx_buffer_is_full(channel_id))
	{
		buffer->pu8_data[buffer->u16_head] = u8_byte;
		buffer->u16_head = (buffer->u16_head + 1) % UART_BUFFER_SIZE;
	}
}

/****************************************************************************************************
 *	Pops a byte off of a given UART's TX buffer, and adjusts the tail index
 *
 * 	@param[in] channel_id The desired UART channel
 *
 *	@note
 *	Called in ISR context
 *
 * 	@return A byte off of the UART's TX buffer
 * 
 ****************************************************************************************************/
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

/****************************************************************************************************
 *	Checks whether a given UART's TX buffer is empty
 *
 * 	@param[in] channel_id The desired UART channel
 *
 *	@note
 *	Called in ISR context
 *
 *	@return `true` if the buffer is empty, else `false`
 ****************************************************************************************************/
static bool UART_tx_buffer_is_empty(UART_channel_id_t channel_id)
{
	UART_buffer_t * buffer = &p_uart_channels[channel_id].tx_buffer;

	return buffer->u16_head == buffer->u16_tail;
}

/****************************************************************************************************
 *	Checks whether a given UART's TX buffer is full
 *
 * 	@param[in] channel_id The desired UART channel
 *
 *	@note
 *	Called in ISR context
 *
 *	@return `true` if the buffer is full, else `false`
 ****************************************************************************************************/
static bool UART_tx_buffer_is_full(UART_channel_id_t channel_id)
{
	UART_buffer_t * buffer = &p_uart_channels[channel_id].tx_buffer;

	return (buffer->u16_head + 1) % UART_BUFFER_SIZE == buffer->u16_tail;
}

/****************************************************************************************************
 *	UART ISR handler utility
 *
 * 	@param[in] channel_id The UART channel whose ISR fired
 *
 *	@note
 *	Called in ISR context
 *
 ****************************************************************************************************/
static void UART_on_isr(UART_channel_id_t channel_id)
{
	UART_channel_t * 					p_channel = &p_uart_channels[channel_id];
	sercom_usart_int_registers_t * 		p_regs = p_channel->_p_sercom_registers;
	volatile uint8_t 					u8_byte;
	BaseType_t 							higher_priority_task_woken = pdFALSE;

	// This flag is cleared by reading the SERCOM_DATA register
	if ((p_regs->SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_RXC(1)) != 0)
	{
		u8_byte = p_regs->SERCOM_DATA;
		UART_rx_buffer_push(channel_id, u8_byte);

		// We pushed a byte onto this uart's buffer. Notify its owner.
		vTaskNotifyGiveFromISR(ARCADIA_handle_from_id(p_channel->owner), &higher_priority_task_woken);
		portYIELD_FROM_ISR(higher_priority_task_woken);
	}

	if ((p_regs->SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_DRE(1)) != 0)
	{
		// No more data to send. Disable interrupts on DRE
		if (UART_tx_buffer_is_empty(channel_id))
		{
			// Don't OR this. INTENCLR is write only.
			p_regs->SERCOM_INTENCLR = SERCOM_USART_INT_INTENCLR_DRE(1);
		}
		// Pop a byte off the TX buffer and send it
		else
		{
			u8_byte = UART_tx_buffer_pop(channel_id);

			p_regs->SERCOM_INTENSET |= SERCOM_USART_INT_INTENSET_TXC(1);

			p_regs->SERCOM_DATA = u8_byte;

			while ((p_regs->SERCOM_INTFLAG & SERCOM_USART_INT_INTENSET_TXC(1)) == 0)
			{
				continue;
			}

			p_regs->SERCOM_INTENCLR = SERCOM_USART_INT_INTENCLR_TXC(1);
		}
	}

	NVIC_ClearPendingIRQ(p_channel->_irq_index);
}

/****************************************************************************************************
 *	SERCOM0 Interrupt Service Routine
 *
 ****************************************************************************************************/
void irqSERCOM0(void)
{
#ifdef DEV_BOARD
	UART_on_isr(UART_CHANNEL_SHELL);
#endif // DEV_BOARD
}

/****************************************************************************************************
 *	SERCOM1 Interrupt Service Routine
 *
 ****************************************************************************************************/
void irqSERCOM1(void)
{
	// Unused
}

/****************************************************************************************************
 *	SERCOM2 Interrupt Service Routine
 *
 ****************************************************************************************************/
void irqSERCOM2(void)
{
	// Unused
}

/****************************************************************************************************
 *	SERCOM3 Interrupt Service Routine
 *
 ****************************************************************************************************/
void irqSERCOM3(void)
{
#ifdef AUDIO_SWITCH
	UART_on_isr(UART_CHANNEL_SHELL);
#endif // AUDIO_SWITCH
}

/****************************************************************************************************
 *	Maps a baud ID to its corresponding BAUD register value according to the configured system clock
 *	frequency.
 *
 * 	Formula for the baud register value is:
 * 	65536 - (65536 * 16 * u32_baud_rate) / SYS_CLOCK_FREQ
 *
 * 	@param[in] baud_id the ID of the desired baud rate
 *
 *	@return the corresponding BAUD register value
 ****************************************************************************************************/
static uint32_t UART_calculate_baud_value(UART_baud_rate_id_t baud_id)
{
	uint32_t u32_sys_clock_freq = SYS_get_source_clock_freq();
	uint64_t u64_numerator = (uint64_t)65536 * 16 * kpu8_baud_rates[baud_id];
	uint32_t u32_baud_value = 65536 - (uint32_t)((u64_numerator + (u32_sys_clock_freq / 2)) / u32_sys_clock_freq);
	return u32_baud_value;
}

/****************************************************************************************************
 *	S H E L L   F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	Shell utility
 *
 * 	Displays UART logical channel info
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t UART_shell_info(uint8_t argc, char ** argv)
{
	UART_channel_t * p_uart;

	if (argc == 0)
	{
		SHELL_SEPARATOR();

		for (uint8_t i = 0; i < UART_CHANNEL_NUM_CHANNELS; i++)
		{
			p_uart = &p_uart_channels[i];

			SHELL_printf("%-20s: %s\r\n", "Channel Name", p_uart->kpc_name);
			SHELL_printf("%-20s: %u\r\n", "Baud Rate",  kpu8_baud_rates[p_uart->baud_rate_id]);
			SHELL_printf("%-20s: %s\r\n", "TX", IO_get_pin_string(p_uart->tx_pin));
			SHELL_printf("%-20s: %s\r\n", "RX", IO_get_pin_string(p_uart->rx_pin));
			
			if (i != (UART_CHANNEL_NUM_CHANNELS - 1))
			{
				SHELL_printf("\n");
			}
		}

		SHELL_SEPARATOR();
	}
	else
	{
		SHELL_printf("Usage: uart info\r\n");
	}

	return SHELL_COMMAND_SUCCESS;
}
