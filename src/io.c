#include "io.h"
#include "utils.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

/**
 *	An enumerated type for PORT groups
 */
typedef enum _IO_group
{
	IO_GROUP_A = 0,
	IO_GROUP_B,
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
		.kcp_name 	= "LCD_DB6",
		.group 		= IO_GROUP_A,
		.u8_number 	= 0
	},
	[IO_PIN_ID_PA01] =
	{
		.kcp_name 	= "LCD_DB7",
		.group 		= IO_GROUP_A,
		.u8_number 	= 1
	},
	[IO_PIN_ID_PA02] =
	{
		.kcp_name 	= "LCD_DB8",
		.group 		= IO_GROUP_A,
		.u8_number 	= 2
	},
	[IO_PIN_ID_PA03] =
	{
		.kcp_name 	= "LCD_DB9",
		.group 		= IO_GROUP_A,
		.u8_number 	= 3
	},
	[IO_PIN_ID_PA04] =
	{
		.kcp_name 	= "LCD_DB16",
		.group 		= IO_GROUP_A,
		.u8_number 	= 4
	},
	[IO_PIN_ID_PA05] =
	{
		.kcp_name 	= "LCD_DB17",
		.group 		= IO_GROUP_A,
		.u8_number 	= 5
	},
	[IO_PIN_ID_PA06] =
	{
		.kcp_name 	= "UART_CHANNEL_SHELL_TX",
		.group 		= IO_GROUP_A,
		.u8_number 	= 6
	},
	[IO_PIN_ID_PA07] =
	{
		.kcp_name 	= "UART_CHANNEL_SHELL_RX",
		.group 		= IO_GROUP_A,
		.u8_number 	= 7
	},
	[IO_PIN_ID_PA08] =
	{
		.kcp_name 	= "BUTTON_A",
		.group 		= IO_GROUP_A,
		.u8_number 	= 8
	},
	[IO_PIN_ID_PA09] =
	{
		.kcp_name 	= "BUTTON_B",
		.group 		= IO_GROUP_A,
		.u8_number 	= 9
	},
	[IO_PIN_ID_PA10] =
	{
		.kcp_name 	= "BUTTON_DPAD_UP",
		.group 		= IO_GROUP_A,
		.u8_number 	= 10
	},
	[IO_PIN_ID_PA11] =
	{
		.kcp_name 	= "BUTTON_DPAD_DOWN",
		.group 		= IO_GROUP_A,
		.u8_number 	= 11
	},
	[IO_PIN_ID_PA12] =
	{
		.kcp_name 	= "BUTTON_DPAD_LEFT",
		.group 		= IO_GROUP_A,
		.u8_number 	= 12
	},
	[IO_PIN_ID_PA13] =
	{
		.kcp_name 	= "BUTTON_DPAD_RIGHT",
		.group 		= IO_GROUP_A,
		.u8_number 	= 13
	},
	[IO_PIN_ID_PA14] =
	{
		.kcp_name 	= "BUTTON_DPAD_MENU",
		.group 		= IO_GROUP_A,
		.u8_number 	= 14
	},
	[IO_PIN_ID_PA15] =
	{
		.kcp_name 	= "SD_DETECT",
		.group 		= IO_GROUP_A,
		.u8_number 	= 15
	},
	[IO_PIN_ID_PA16] =
	{
		.kcp_name 	= "SPI_CHANNEL_SD_CARD_DATA_IN",
		.group 		= IO_GROUP_A,
		.u8_number 	= 16
	},
	[IO_PIN_ID_PA17] =
	{
		.kcp_name 	= "SPI_CHANNEL_SD_CARD_SS",
		.group 		= IO_GROUP_A,
		.u8_number 	= 17
	},
	[IO_PIN_ID_PA18] =
	{
		.kcp_name 	= "SPI_CHANNEL_SD_CARD_DATA_OUT",
		.group 		= IO_GROUP_A,
		.u8_number 	= 18
	},
	[IO_PIN_ID_PA19] =
	{
		.kcp_name 	= "SPI_CHANNEL_SD_CARD_CLOCK",
		.group 		= IO_GROUP_A,
		.u8_number 	= 19
	},
	[IO_PIN_ID_PA20] =
	{
		.kcp_name 	= "LCD_CS",
		.group 		= IO_GROUP_A,
		.u8_number 	= 20
	},
	[IO_PIN_ID_PA21] =
	{
		.kcp_name 	= "PA21",
		.group 		= IO_GROUP_A,
		.u8_number 	= 21
	},
	[IO_PIN_ID_PA22] =
	{
		.kcp_name 	= "PA22",
		.group 		= IO_GROUP_A,
		.u8_number 	= 22
	},
	[IO_PIN_ID_PA23] =
	{
		.kcp_name 	= "PA23",
		.group 		= IO_GROUP_A,
		.u8_number 	= 23
	},
	[IO_PIN_ID_PA24] =
	{
		.kcp_name 	= "PA24",
		.group 		= IO_GROUP_A,
		.u8_number 	= 24
	},
	[IO_PIN_ID_PA25] =
	{
		.kcp_name 	= "PA25",
		.group 		= IO_GROUP_A,
		.u8_number 	= 25
	},
	[IO_PIN_ID_PA27] =
	{
		.kcp_name 	= "LCD_WR",
		.group 		= IO_GROUP_A,
		.u8_number 	= 27
	},
	[IO_PIN_ID_PA28] =
	{
		.kcp_name 	= "LCD_RD",
		.group 		= IO_GROUP_A,
		.u8_number 	= 28
	},
	[IO_PIN_ID_PA30] =
	{
		.kcp_name 	= "SWCLK",
		.group 		= IO_GROUP_A,
		.u8_number 	= 30
	},
	[IO_PIN_ID_PA31] =
	{
		.kcp_name 	= "SWDIO",
		.group 		= IO_GROUP_A,
		.u8_number 	= 31
	},

	/////

	[IO_PIN_ID_PB00] =
	{
		.kcp_name 	= "LCD_DB2",
		.group 		= IO_GROUP_B,
		.u8_number 	= 0
	},
	[IO_PIN_ID_PB01] =
	{
		.kcp_name 	= "LCD_DB3",
		.group 		= IO_GROUP_B,
		.u8_number 	= 1
	},
	[IO_PIN_ID_PB02] =
	{
		.kcp_name 	= "LCD_DB4",
		.group 		= IO_GROUP_B,
		.u8_number 	= 2
	},
	[IO_PIN_ID_PB03] =
	{
		.kcp_name 	= "LCD_DB5",
		.group 		= IO_GROUP_B,
		.u8_number 	= 3
	},
	[IO_PIN_ID_PB04] =
	{
		.kcp_name 	= "LCD_DB10",
		.group 		= IO_GROUP_B,
		.u8_number 	= 4
	},
	[IO_PIN_ID_PB05] =
	{
		.kcp_name 	= "LCD_DB11",
		.group 		= IO_GROUP_B,
		.u8_number 	= 5
	},
	[IO_PIN_ID_PB06] =
	{
		.kcp_name 	= "LCD_DB12",
		.group 		= IO_GROUP_B,
		.u8_number 	= 6
	},
	[IO_PIN_ID_PB07] =
	{
		.kcp_name 	= "LCD_DB13",
		.group 		= IO_GROUP_B,
		.u8_number 	= 7
	},
	[IO_PIN_ID_PB08] =
	{
		.kcp_name 	= "LCD_DB14",
		.group 		= IO_GROUP_B,
		.u8_number 	= 8
	},
	[IO_PIN_ID_PB09] =
	{
		.kcp_name 	= "LCD_DB15",
		.group 		= IO_GROUP_B,
		.u8_number 	= 9
	},
	[IO_PIN_ID_PB10] =
	{
		.kcp_name 	= "LCD_IM2",
		.group 		= IO_GROUP_B,
		.u8_number 	= 10
	},
	[IO_PIN_ID_PB11] =
	{
		.kcp_name 	= "LCD_IM1",
		.group 		= IO_GROUP_B,
		.u8_number 	= 11
	},
	[IO_PIN_ID_PB12] =
	{
		.kcp_name 	= "LCD_IM0",
		.group 		= IO_GROUP_B,
		.u8_number 	= 12
	},
	[IO_PIN_ID_PB13] =
	{
		.kcp_name 	= "LCD_HSYNC",
		.group 		= IO_GROUP_B,
		.u8_number 	= 13
	},
	[IO_PIN_ID_PB14] =
	{
		.kcp_name 	= "LCD_VSYNC",
		.group 		= IO_GROUP_B,
		.u8_number 	= 14
	},
	[IO_PIN_ID_PB15] =
	{
		.kcp_name 	= "LCD_PCLK",
		.group 		= IO_GROUP_B,
		.u8_number 	= 15
	},
	[IO_PIN_ID_PB16] =
	{
		.kcp_name 	= "LCD_DE",
		.group 		= IO_GROUP_B,
		.u8_number 	= 16
	},
	[IO_PIN_ID_PB17] =
	{
		.kcp_name 	= "LCD_RESET",
		.group 		= IO_GROUP_B,
		.u8_number 	= 17
	},
	[IO_PIN_ID_PB22] =
	{
		.kcp_name 	= "USB_5V",
		.group 		= IO_GROUP_B,
		.u8_number 	= 22
	},
	[IO_PIN_ID_PB23] =
	{
		.kcp_name 	= "LCD_RS",
		.group 		= IO_GROUP_B,
		.u8_number 	= 23
	},
	[IO_PIN_ID_PB30] =
	{
		.kcp_name 	= "LCD_DB0",
		.group 		= IO_GROUP_B,
		.u8_number 	= 30
	},
	[IO_PIN_ID_PB31] =
	{
		.kcp_name 	= "LCD_DB1",
		.group 		= IO_GROUP_B,
		.u8_number 	= 31
	}
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
	ASSERT(pin_id < IO_PIN_ID_NUM_PINS);
	ASSERT(peripheral_function < IO_PERIPHERAL_FUNCTION_NUM_FUNCTIONS);

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
	ASSERT(pin_id < IO_PIN_ID_NUM_PINS);
	ASSERT(peripheral_function < IO_PERIPHERAL_FUNCTION_NUM_FUNCTIONS);

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
	ASSERT(pin_id < IO_PIN_ID_NUM_PINS);
	ASSERT(direction < IO_DIRECTION_NUM_DIRECTIONS);

	IO_pin_t pin = pin_map[pin_id];

	PORT_REGS->GROUP[pin.group].PORT_DIR |= direction << pin.u8_number;
}

/****************************************************************************************************
 *	Set an IO pin high or low
 *  
 *	@param[in] pin_id The ID of the desired pin
 *	@param[in] state High or low
 *
 *	@note Performance-critical. Not asserting on params
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
 *	@note Performance-critical. Not asserting on params
 *
 *	@return The logical level of the pin
 ****************************************************************************************************/
IO_pin_state_t IO_read_pin(IO_pin_id_t pin_id)
{
	IO_pin_t pin = pin_map[pin_id];

	return (IO_pin_state_t)PORT_REGS->GROUP[pin.group].PORT_IN & (1 << pin.u8_number);
}

/****************************************************************************************************
 *	Enables internal pullup for the given pin
 *  
 *	@param[in] pin_id The ID of the desired pin
 *
 ****************************************************************************************************/
void IO_enable_pullup(IO_pin_id_t pin_id)
{
	ASSERT(pin_id < IO_PIN_ID_NUM_PINS);

	IO_pin_t pin = pin_map[pin_id];

	PORT_REGS->GROUP[pin.group].PORT_PINCFG[pin.u8_number] |= PORT_PINCFG_PULLEN(1);
}

/****************************************************************************************************
 *	Disables internal pullup for the given pin
 *  
 *	@param[in] pin_id The ID of the desired pin
 *
 ****************************************************************************************************/
void IO_disable_pullup(IO_pin_id_t pin_id)
{
	ASSERT(pin_id < IO_PIN_ID_NUM_PINS);

	IO_pin_t pin = pin_map[pin_id];

	PORT_REGS->GROUP[pin.group].PORT_PINCFG[pin.u8_number] &= ~PORT_PINCFG_PULLEN(1);
}

/****************************************************************************************************
 *	Enables stronger drive strength for the given pin
 *  
 *	@param[in] pin_id The ID of the desired pin
 *
 ****************************************************************************************************/
void IO_enable_strong_drive_strength(IO_pin_id_t pin_id)
{
	ASSERT(pin_id < IO_PIN_ID_NUM_PINS);

	IO_pin_t pin = pin_map[pin_id];

	PORT_REGS->GROUP[pin.group].PORT_PINCFG[pin.u8_number] |= PORT_PINCFG_DRVSTR(1);
}

/****************************************************************************************************
 *	Disables stronger drive strength for the given pin
 *  
 *	@param[in] pin_id The ID of the desired pin
 *
 ****************************************************************************************************/
void IO_disable_strong_drive_strength(IO_pin_id_t pin_id)
{
	ASSERT(pin_id < IO_PIN_ID_NUM_PINS);

	IO_pin_t pin = pin_map[pin_id];

	PORT_REGS->GROUP[pin.group].PORT_PINCFG[pin.u8_number] &= ~PORT_PINCFG_DRVSTR(1);
}

/****************************************************************************************************
 *	Retrieves the string descriptor of the given pin according to its entry in the pin map
 *  
 *	@param[in] pin_id The ID of the desired pin
 *
 * 	@return The name of the pin
 * 
 ****************************************************************************************************/
const char * IO_get_pin_name(IO_pin_id_t pin_id)
{
	ASSERT(pin_id < IO_PIN_ID_NUM_PINS);

	return pin_map[pin_id].kcp_name;
}
