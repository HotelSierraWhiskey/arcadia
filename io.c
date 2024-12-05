#include "io.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

/**
 *	An enumerated type for PORT groups
 *	
 *	@note 
 *	The SAMC21E18A implements PORT A only
 */
typedef enum _IO_group
{
	IO_GROUP_A = 0,
	//////////
	IO_GROUP_NUM_GROUPS
} IO_group_t;

/**
 *	A pin typedef where all information relevant to a given pin is tracked
 */
typedef struct _IO_pin
{
	const char *	kcp_name;
	IO_group_t		group;
	uint8_t			u8_number;
} IO_pin_t;

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

/**
 *	The application pin map
 */
static const IO_pin_t pin_map[IO_PIN_ID_NUM_PINS] =
{
	[IO_PIN_ID_PA00] =
	{
		.kcp_name = "PA00",
		.group = IO_GROUP_A,
		.u8_number = 0
	},
	[IO_PIN_ID_PA01] =
	{
		.kcp_name = "PA01",
		.group = IO_GROUP_A,
		.u8_number = 1
	},
	[IO_PIN_ID_PA02] =
	{
		.kcp_name = "PA02",
		.group = IO_GROUP_A,
		.u8_number = 2
	},
	[IO_PIN_ID_PA03] =
	{
		.kcp_name = "PA03",
		.group = IO_GROUP_A,
		.u8_number = 3
	},
	[IO_PIN_ID_PA04] =
	{
		.kcp_name = "PA04",
		.group = IO_GROUP_A,
		.u8_number = 4
	},
	[IO_PIN_ID_PA05] =
	{
		.kcp_name = "PA05",
		.group = IO_GROUP_A,
		.u8_number = 5
	},
	[IO_PIN_ID_PA06] =
	{
		.kcp_name = "PA06",
		.group = IO_GROUP_A,
		.u8_number = 6
	},
	[IO_PIN_ID_PA07] =
	{
		.kcp_name = "PA07",
		.group = IO_GROUP_A,
		.u8_number = 7
	},
	[IO_PIN_ID_PA08] =
	{
		.kcp_name = "PA08",
		.group = IO_GROUP_A,
		.u8_number = 8
	},
	[IO_PIN_ID_PA09] =
	{
		.kcp_name = "PA09",
		.group = IO_GROUP_A,
		.u8_number = 9
	},
	[IO_PIN_ID_PA10] =
	{
		.kcp_name = "PA10",
		.group = IO_GROUP_A,
		.u8_number = 10
	},
	[IO_PIN_ID_PA11] =
	{
		.kcp_name = "PA11",
		.group = IO_GROUP_A,
		.u8_number = 11
	},
	[IO_PIN_ID_PA14] =
	{
		.kcp_name = "PA14",
		.group = IO_GROUP_A,
		.u8_number = 14
	},
	[IO_PIN_ID_PA15] =
	{
		.kcp_name = "PA15",
		.group = IO_GROUP_A,
		.u8_number = 15
	},
	[IO_PIN_ID_PA16] =
	{
		.kcp_name = "PA16",
		.group = IO_GROUP_A,
		.u8_number = 16
	},
	[IO_PIN_ID_PA17] =
	{
		.kcp_name = "PA17",
		.group = IO_GROUP_A,
		.u8_number = 17
	},
	[IO_PIN_ID_PA18] =
	{
		.kcp_name = "PA18",
		.group = IO_GROUP_A,
		.u8_number = 18
	},
	[IO_PIN_ID_PA19] =
	{
		.kcp_name = "PA19",
		.group = IO_GROUP_A,
		.u8_number = 19
	},
	[IO_PIN_ID_PA22] =
	{
		.kcp_name = "PA22",
		.group = IO_GROUP_A,
		.u8_number = 22
	},
	[IO_PIN_ID_PA23] =
	{
		.kcp_name = "PA23",
		.group = IO_GROUP_A,
		.u8_number = 23
	},
	[IO_PIN_ID_PA24] =
	{
		.kcp_name = "PA24",
		.group = IO_GROUP_A,
		.u8_number = 24
	},
	[IO_PIN_ID_PA25] =
	{
		.kcp_name = "PA25",
		.group = IO_GROUP_A,
		.u8_number = 25
	},
	[IO_PIN_ID_PA27] =
	{
		.kcp_name = "PA27",
		.group = IO_GROUP_A,
		.u8_number = 27
	},
	[IO_PIN_ID_PA28] =
	{
		.kcp_name = "PA28",
		.group = IO_GROUP_A,
		.u8_number = 28
	},
	[IO_PIN_ID_PA30] =
	{
		.kcp_name = "PA30",
		.group = IO_GROUP_A,
		.u8_number = 30
	},
	[IO_PIN_ID_PA31] =
	{
		.kcp_name = "PA31",
		.group = IO_GROUP_A,
		.u8_number = 31
	},
};

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	Initializes the IO Module
 *
 ****************************************************************************************************/
void IO_init(void)
{
	// Set the APB for the PORT peripheral
	MCLK_REGS->MCLK_APBBMASK |= MCLK_APBBMASK_PORT(1);
}

/****************************************************************************************************
 *	Enables a peripheral function for the given pin
 *  
 *	@param[in] pin_id The ID of the desired pin
 *	@param[in] peripheral_function The peripheral function to map to the pin
 *
 ****************************************************************************************************/
void IO_enable_peripheral_function_for_pin(IO_pin_id_t pin_id, IO_peripheral_function_t peripheral_function)
{
	IO_pin_t pin = pin_map[pin_id];
	bool b_odd = pin.u8_number & 1;
	volatile uint8_t * u8_pmux_register = &PORT_REGS->GROUP[pin.group].PORT_PMUX[pin.u8_number / 2];

	// Clear PMUXO/ PMUXE bits and set the new peripheral function
	if (b_odd)
	{
		*u8_pmux_register = (*u8_pmux_register & ~PORT_PMUX_PMUXO_Msk) | PORT_PMUX_PMUXO(peripheral_function);
	}
	else
	{
		*u8_pmux_register = (*u8_pmux_register & ~PORT_PMUX_PMUXE_Msk) | PORT_PMUX_PMUXE(peripheral_function);
	}

	// Enable the peripheral multiplexer for the pin
	PORT_REGS->GROUP[pin.group].PORT_PINCFG[pin.u8_number] |= PORT_PINCFG_PMUXEN(1);
}

/****************************************************************************************************
 *	Disable a peripheral function for the given pin
 *  
 *	@param[in] pin_id The ID of the desired pin
 *	@param[in] peripheral_function The peripheral function for which to disable the pin
 *
 ****************************************************************************************************/
void IO_disable_peripheral_function_for_pin(IO_pin_id_t pin_id, IO_peripheral_function_t peripheral_function)
{
	IO_pin_t pin = pin_map[pin_id];
	bool b_odd = pin.u8_number & 1;
	volatile uint8_t * u8_pmux_register = &PORT_REGS->GROUP[pin.group].PORT_PMUX[pin.u8_number / 2];

	// Clear the PMUXO/ PMUXE bits associated with the pin
	if (b_odd)
	{
		*u8_pmux_register &= ~PORT_PMUX_PMUXO_Msk;
	}
	else
	{
		*u8_pmux_register &= ~PORT_PMUX_PMUXE_Msk;
	}

	// Disable the peripheral multiplexer for the pin
	PORT_REGS->GROUP[pin.group].PORT_PINCFG[pin.u8_number] &= ~PORT_PINCFG_PMUXEN(1);
}

/****************************************************************************************************
 *	Configure an IO pin for either input or output
 *  
 *	@param[in] pin_id The ID of the desired pin
 *	@param[in] direction Input or output
 *
 ****************************************************************************************************/
void IO_config_pin_direction(IO_pin_id_t pin_id, IO_pin_direction_t direction)
{
	// TODO: asserts
	IO_pin_t pin = pin_map[pin_id];

	PORT_REGS->GROUP[pin.group].PORT_DIR |= direction << pin.u8_number;
}

/****************************************************************************************************
 *	Set an IO pin high or low
 *  
 *	@param[in] pin_id The ID of the desired pin
 *	@param[in] state High or low
 *
 ****************************************************************************************************/
void IO_set_pin(IO_pin_id_t pin_id, IO_pin_state_t state)
{
	IO_pin_t pin = pin_map[pin_id];

	if (state == IO_PIN_STATE_HIGH)
	{
		PORT_REGS->GROUP[pin.group].PORT_OUTSET |= state << pin.u8_number;
	}
	else
	{
		PORT_REGS->GROUP[pin.group].PORT_OUTCLR |= state << pin.u8_number;
	}
}

/****************************************************************************************************
 *	Reads the signal level of a given pin
 *  
 *	@param[in] pin_id The ID of the desired pin
 *
 *	@return The logical level of the pin
 ****************************************************************************************************/
IO_pin_state_t IO_read_pin(IO_pin_id_t pin_id)
{
	IO_pin_t pin = pin_map[pin_id];

	return (IO_pin_state_t)PORT_REGS->GROUP[pin.group].PORT_IN & (1 << pin.u8_number);
}

void IO_enable_pullup(IO_pin_id_t pin_id)
{
	IO_pin_t pin = pin_map[pin_id];

	PORT_REGS->GROUP[pin.group].PORT_PINCFG[pin.u8_number] |= PORT_PINCFG_PULLEN(1);
}

void IO_disable_pullup(IO_pin_id_t pin_id)
{
	IO_pin_t pin = pin_map[pin_id];

	PORT_REGS->GROUP[pin.group].PORT_PINCFG[pin.u8_number] &= ~PORT_PINCFG_PULLEN(1);
}
