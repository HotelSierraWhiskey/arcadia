#include "spi.h"
#include "sercom.h"
#include "io.h"
#include "arcadia.h"
#include "utils.h"
#include "chrono.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define SPI_400KHZ	(59)

/**
 *	Logical API channel typedef
 */
typedef struct _SPI_channel
{
	// Must be populated
	const char *					kpc_name;
	SERCOM_channel_id_t				sercom_channel_id;
	IO_pin_id_t						data_out_pin;
	IO_pin_id_t						data_in_pin;
	IO_pin_id_t						clock_pin;
	IO_pin_id_t						ss_pin;
	uint8_t							dipo_setting;
	uint8_t							dopo_setting;
	IO_peripheral_function_t		peripheral_function;
	
	// Populated during initialization (derived from `sercom_channel_id`)
	IRQn_Type						_irq_index;
	volatile sercom_registers_t	*	_p_sercom_registers;
} SPI_channel_t;

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

/**
 *	SPI channels
 *
 * 	A registry of logical SPI channels
 * 
 * 	@note
 * 	Supports one slave device per channel for now
 * 	SS is always manual
 * 
 */
static SPI_channel_t p_spi_channels[SPI_CHANNEL_NUM_CHANNELS] =
{
	[SPI_CHANNEL_SD_CARD] =
	{
		.kpc_name 				= "SD Card",
		.sercom_channel_id 		= SERCOM_CHANNEL_ID_1,
		.data_out_pin			= IO_PIN_ID_PA18,
		.data_in_pin			= IO_PIN_ID_PA16,
		.clock_pin				= IO_PIN_ID_PA19,
		.ss_pin					= IO_PIN_ID_PA17,
		.dipo_setting			= 0x00,
		.dopo_setting			= 0x01,
		.peripheral_function 	= IO_PERIPHERAL_FUNCTION_C
	}
};

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	SPI channel initialization (Master Mode)
 *
 * 	Initializes to 400KHz baud by default.
 *
 * 	@param[in] channel_id The logical SPI channel to initialize
 *
 ****************************************************************************************************/
void SPI_init(SPI_channel_id_t channel_id)
{
	SPI_channel_t * 	p_channel = &p_spi_channels[channel_id];
	uint8_t 			u8_PCHCTRL_register_index = SERCOM_get_PCHCTRL_register_index(p_channel->sercom_channel_id);

	GCLK_REGS->GCLK_PCHCTRL[u8_PCHCTRL_register_index] = 	GCLK_PCHCTRL_CHEN(1) | 
															GCLK_PCHCTRL_GEN_GCLK0;

	switch (p_channel->sercom_channel_id)
	{
		case SERCOM_CHANNEL_ID_0:
			MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_SERCOM0(1);
			p_channel->_irq_index = SERCOM0_IRQn;
			p_channel->_p_sercom_registers = SERCOM0_REGS;
			break;
		case SERCOM_CHANNEL_ID_1:
			MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_SERCOM1(1);
			p_channel->_irq_index = SERCOM1_IRQn;
			p_channel->_p_sercom_registers = SERCOM1_REGS;
			break;
		case SERCOM_CHANNEL_ID_2:
			MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_SERCOM2(1);
			p_channel->_irq_index = SERCOM2_IRQn;
			p_channel->_p_sercom_registers = SERCOM2_REGS;
			break;
		case SERCOM_CHANNEL_ID_3:
			MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_SERCOM3(1);
			p_channel->_irq_index = SERCOM3_IRQn;
			p_channel->_p_sercom_registers = SERCOM3_REGS;
			break;
	}

	MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_SERCOM1(1);

	while ((MCLK_REGS->MCLK_INTFLAG & MCLK_INTFLAG_CKRDY(1)) == 0)
	{
		continue;
	}

	IO_enable_peripheral_function_for_pin(p_channel->data_in_pin, p_channel->peripheral_function);
	IO_enable_peripheral_function_for_pin(p_channel->data_out_pin, p_channel->peripheral_function);
	IO_enable_peripheral_function_for_pin(p_channel->clock_pin, p_channel->peripheral_function);
	IO_config_pin_direction(p_channel->ss_pin, IO_DIRECTION_OUTPUT);
	SPI_ss_pin_high(channel_id);

	p_channel->_p_sercom_registers->SPIM.SERCOM_CTRLB =	SERCOM_SPIM_CTRLB_RXEN(1);

	p_channel->_p_sercom_registers->SPIM.SERCOM_BAUD = SPI_400KHZ;

	p_channel->_p_sercom_registers->SPIM.SERCOM_INTENSET = SERCOM_SPIM_INTENSET_RXC(1);

	p_channel->_p_sercom_registers->SPIM.SERCOM_CTRLA = 	SERCOM_SPIM_CTRLA_MODE_SPI_MASTER |
															SERCOM_SPIM_CTRLA_DOPO(1) |
															SERCOM_SPIM_CTRLA_ENABLE(1);

	while (p_channel->_p_sercom_registers->SPIM.SERCOM_SYNCBUSY & SERCOM_SPIM_SYNCBUSY_ENABLE(1))
	{
		continue;
	}
}

void SPI_set_baud(SPI_channel_id_t channel_id, SPI_baud_id_t baud_id)
{
	ASSERT(baud_id < SPI_BAUD_ID_NUM_IDS);

	SPI_channel_t * 	p_channel = &p_spi_channels[channel_id];
	uint32_t 			u32_baud_val;

	switch (baud_id)
	{
		case SPI_BAUD_ID_400KHZ:
			u32_baud_val = 59;
			break;

		case SPI_BAUD_ID_25MHZ:
			u32_baud_val = 0;
			break;

		default:
			u32_baud_val = 59;
	}

	p_channel->_p_sercom_registers->SPIM.SERCOM_CTRLA &= ~SERCOM_SPIM_CTRLA_ENABLE(1);

	while (p_channel->_p_sercom_registers->SPIM.SERCOM_SYNCBUSY & SERCOM_SPIM_SYNCBUSY_ENABLE(1))
	{
		continue;
	}
	
	p_channel->_p_sercom_registers->SPIM.SERCOM_BAUD = u32_baud_val;
	
	p_channel->_p_sercom_registers->SPIM.SERCOM_CTRLA |= SERCOM_SPIM_CTRLA_ENABLE(1);
	
	while (p_channel->_p_sercom_registers->SPIM.SERCOM_SYNCBUSY & SERCOM_SPIM_SYNCBUSY_ENABLE(1))
	{
		continue;
	}
}

/****************************************************************************************************
 *	Sends a byte over the selected SPI interface, and returns the received data
 *
 * 	@note SS pin must be manually pulled high/ low.
 *
 * 	@param[in] channel_id The logical channel over which to transfer data
 * 	@param[in] u8_byte The data to send
 *
 * 	@return data from the addressed slave device
 ****************************************************************************************************/
uint8_t SPI_transfer(SPI_channel_id_t channel_id, uint8_t u8_byte)
{
	p_spi_channels[channel_id]._p_sercom_registers->SPIM.SERCOM_DATA = u8_byte;

	while ((p_spi_channels[channel_id]._p_sercom_registers->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_DRE(1)) == 0)
	{
		continue;
	}

	while ((p_spi_channels[channel_id]._p_sercom_registers->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_RXC(1)) == 0)
    {
        continue;
    }

	return p_spi_channels[channel_id]._p_sercom_registers->SPIM.SERCOM_DATA;
}

/****************************************************************************************************
 *	Pulls the selected SS pin high
 *
 * 	@param[in] channel_id The desired SPI channel
 *
 ****************************************************************************************************/
void SPI_ss_pin_high(SPI_channel_id_t channel_id)
{
	IO_set_pin(p_spi_channels[channel_id].ss_pin, IO_PIN_STATE_HIGH);
}

/****************************************************************************************************
 *	Pulls the selected SS pin low
 *
 * 	@param[in] channel_id The desired SPI channel
 *
 ****************************************************************************************************/
void SPI_ss_pin_low(SPI_channel_id_t channel_id)
{
	IO_set_pin(p_spi_channels[channel_id].ss_pin, IO_PIN_STATE_LOW);
}

/****************************************************************************************************
 *	S H E L L   F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	Shell utility
 *
 * 	Sends a given number of bytes over the selected SPI channel
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t	SPI_shell_write(uint8_t argc, char ** argv)
{
	uint32_t 		u32_num_bytes;
	uint32_t 		u32_channel_id;
	const uint8_t 	ku8_max_bytes = 8;
	uint32_t		u32_data[ku8_max_bytes];
	bool 			b_res = true;

	if (argc >= 3)
	{
		if (UTILS_string_to_u32(argv[0], &u32_channel_id))
		{
			if (u32_channel_id > SPI_CHANNEL_NUM_CHANNELS)
			{
				b_res = false;
			}
		}
		if (b_res && UTILS_string_to_u32(argv[1], &u32_num_bytes))
		{
			if (u32_num_bytes > ku8_max_bytes)
			{
				b_res = false;
			}
		}
		if (b_res)
		{
			for (uint8_t i = 0; i < u32_num_bytes; i++)
			{
				if (UTILS_string_to_u32(argv[2 + i], &u32_data[i]) == false)
				{
					b_res = false;
					SHELL_printf("Error: %s", argv[2 + i]);
					break;
				}
			}
		}
	}
	
	if (b_res)
	{
		for (uint8_t i = 0; i < u32_num_bytes; i++)
		{
			SPI_ss_pin_low(u32_channel_id);
			SPI_transfer((SPI_channel_id_t)u32_channel_id, u32_data[i]);
			SPI_ss_pin_high(u32_channel_id);
		}
	}
	else
	{
		SHELL_printf("Usage: spi write <channel_id> <num_bytes> <...>");
	}

	return SHELL_COMMAND_SUCCESS;
}
