#include "io.h"
#include "utils.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define IO_PIN_STRING_DESCRIPTOR_SIZE (5U)

/**
 *	An enumerated type for PORT groups
 */
typedef enum _IO_group
{
	IO_GROUP_A = 0,
	IO_GROUP_B,
	IO_GROUP_C,
	//////////
	IO_GROUP_NUM_GROUPS
} IO_group_t;

/**
 *	A pin typedef where all information relevant to a given pin is tracked
 */
typedef struct _IO_pin
{
	const char *	kpc_name;
	char			pc_string[IO_PIN_STRING_DESCRIPTOR_SIZE];
	IO_group_t		group;
	uint8_t			u8_number;
	IO_pin_type_t	type;
} IO_pin_t;

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

/**
 *	Main application pin map
 */
#ifdef DEV_BOARD
static IO_pin_t p_pin_map[IO_PIN_ID_NUM_PINS] =
{
	// Port A

	[IO_PIN_ID_PA00] =
	{
		.kpc_name 	= "EXTI_DBG",
		.group 		= IO_GROUP_A,
		.u8_number 	= 0,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA01] =
	{
		.kpc_name 	= "LCD_DB7",
		.group 		= IO_GROUP_A,
		.u8_number 	= 1,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA02] =
	{
		.kpc_name 	= "DAC_VOUT",
		.group 		= IO_GROUP_A,
		.u8_number 	= 2,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA03] =
	{
		.kpc_name 	= "LCD_DB9",
		.group 		= IO_GROUP_A,
		.u8_number 	= 3,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA04] =
	{
		.kpc_name 	= "LCD_DB16",
		.group 		= IO_GROUP_A,
		.u8_number 	= 4,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA05] =
	{
		.kpc_name 	= "LCD_RESET",
		.group 		= IO_GROUP_A,
		.u8_number 	= 5,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA06] =
	{
		.kpc_name 	= "UART_CHANNEL_SHELL_TX",
		.group 		= IO_GROUP_A,
		.u8_number 	= 6,
		.type		= IO_PIN_TYPE_PERIPHERAL
	},
	[IO_PIN_ID_PA07] =
	{
		.kpc_name 	= "UART_CHANNEL_SHELL_RX",
		.group 		= IO_GROUP_A,
		.u8_number 	= 7,
		.type		= IO_PIN_TYPE_PERIPHERAL
	},
	[IO_PIN_ID_PA08] =
	{
		.kpc_name 	= "BUTTON_A",
		.group 		= IO_GROUP_A,
		.u8_number 	= 8,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA09] =
	{
		.kpc_name 	= "BUTTON_B",
		.group 		= IO_GROUP_A,
		.u8_number 	= 9,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA10] =
	{
		.kpc_name 	= "BUTTON_DPAD_UP",
		.group 		= IO_GROUP_A,
		.u8_number 	= 10,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA11] =
	{
		.kpc_name 	= "BUTTON_DPAD_DOWN",
		.group 		= IO_GROUP_A,
		.u8_number 	= 11,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA12] =
	{
		.kpc_name 	= "BUTTON_DPAD_LEFT",
		.group 		= IO_GROUP_A,
		.u8_number 	= 12,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA13] =
	{
		.kpc_name 	= "BUTTON_DPAD_RIGHT",
		.group 		= IO_GROUP_A,
		.u8_number 	= 13,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA14] =
	{
		.kpc_name 	= "GCLK_IO_0",
		.group 		= IO_GROUP_A,
		.u8_number 	= 14,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA15] =
	{
		.kpc_name 	= "SD_DETECT",
		.group 		= IO_GROUP_A,
		.u8_number 	= 15,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA16] =
	{
		.kpc_name 	= "SPI_CHANNEL_SD_CARD_DATA_IN",
		.group 		= IO_GROUP_A,
		.u8_number 	= 16,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA17] =
	{
		.kpc_name 	= "SPI_CHANNEL_SD_CARD_SS",
		.group 		= IO_GROUP_A,
		.u8_number 	= 17,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA18] =
	{
		.kpc_name 	= "SPI_CHANNEL_SD_CARD_DATA_OUT",
		.group 		= IO_GROUP_A,
		.u8_number 	= 18,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA19] =
	{
		.kpc_name 	= "SPI_CHANNEL_SD_CARD_CLOCK",
		.group 		= IO_GROUP_A,
		.u8_number 	= 19,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA20] =
	{
		.kpc_name 	= "LCD_CS",
		.group 		= IO_GROUP_A,
		.u8_number 	= 20,
		.type		= IO_PIN_TYPE_OUTPUT
	},
	[IO_PIN_ID_PA21] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 21,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA22] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 22,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA23] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 23,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA24] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 24,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA25] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 25,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA27] =
	{
		.kpc_name 	= "LCD_WR",
		.group 		= IO_GROUP_A,
		.u8_number 	= 27,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA28] =
	{
		.kpc_name 	= "LCD_RD",
		.group 		= IO_GROUP_A,
		.u8_number 	= 28,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA30] =
	{
		.kpc_name 	= "SWCLK",
		.group 		= IO_GROUP_A,
		.u8_number 	= 30,
		.type		= IO_PIN_TYPE_PERIPHERAL
	},
	[IO_PIN_ID_PA31] =
	{
		.kpc_name 	= "SWDIO",
		.group 		= IO_GROUP_A,
		.u8_number 	= 31,
		.type		= IO_PIN_TYPE_PERIPHERAL
	},

	// Port B

	[IO_PIN_ID_PB00] =
	{
		.kpc_name 	= "LCD_DB2",
		.group 		= IO_GROUP_B,
		.u8_number 	= 0,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB01] =
	{
		.kpc_name 	= "LCD_DB3",
		.group 		= IO_GROUP_B,
		.u8_number 	= 1,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB02] =
	{
		.kpc_name 	= "LCD_DB4",
		.group 		= IO_GROUP_B,
		.u8_number 	= 2,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB03] =
	{
		.kpc_name 	= "LCD_DB5",
		.group 		= IO_GROUP_B,
		.u8_number 	= 3,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB04] =
	{
		.kpc_name 	= "LCD_DB10",
		.group 		= IO_GROUP_B,
		.u8_number 	= 4,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB05] =
	{
		.kpc_name 	= "LCD_DB11",
		.group 		= IO_GROUP_B,
		.u8_number 	= 5,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB06] =
	{
		.kpc_name 	= "LCD_DB12",
		.group 		= IO_GROUP_B,
		.u8_number 	= 6,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB07] =
	{
		.kpc_name 	= "LCD_DB13",
		.group 		= IO_GROUP_B,
		.u8_number 	= 7,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB08] =
	{
		.kpc_name 	= "LCD_DB14",
		.group 		= IO_GROUP_B,
		.u8_number 	= 8,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB09] =
	{
		.kpc_name 	= "LCD_DB15",
		.group 		= IO_GROUP_B,
		.u8_number 	= 9,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB10] =
	{
		.kpc_name 	= "LCD_IM2",
		.group 		= IO_GROUP_B,
		.u8_number 	= 10,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB11] =
	{
		.kpc_name 	= "LCD_IM1",
		.group 		= IO_GROUP_B,
		.u8_number 	= 11,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB12] =
	{
		.kpc_name 	= "LCD_IM0",
		.group 		= IO_GROUP_B,
		.u8_number 	= 12,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB13] =
	{
		.kpc_name 	= "LCD_HSYNC",
		.group 		= IO_GROUP_B,
		.u8_number 	= 13,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB14] =
	{
		.kpc_name 	= "LCD_VSYNC",
		.group 		= IO_GROUP_B,
		.u8_number 	= 14,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB15] =
	{
		.kpc_name 	= "LCD_PCLK",
		.group 		= IO_GROUP_B,
		.u8_number 	= 15,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB16] =
	{
		.kpc_name 	= "LCD_DE",
		.group 		= IO_GROUP_B,
		.u8_number 	= 16,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB17] =
	{
		.kpc_name 	= "LCD_RESET",
		.group 		= IO_GROUP_B,
		.u8_number 	= 17,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB22] =
	{
		.kpc_name 	= "USB_5V",
		.group 		= IO_GROUP_B,
		.u8_number 	= 22,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB23] =
	{
		.kpc_name 	= "LCD_RS",
		.group 		= IO_GROUP_B,
		.u8_number 	= 23,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB30] =
	{
		.kpc_name 	= "LCD_DB0",
		.group 		= IO_GROUP_B,
		.u8_number 	= 30,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB31] =
	{
		.kpc_name 	= "LCD_DB1",
		.group 		= IO_GROUP_B,
		.u8_number 	= 31,
		.type		= IO_PIN_TYPE_UNUSED
	}
};
#endif // DEV_BOARD

#ifdef AUDIO_SWITCH
static IO_pin_t p_pin_map[IO_PIN_ID_NUM_PINS] =
{
	// Port A

	[IO_PIN_ID_PA00] =
	{
		.kpc_name 	= "EXTI_DBG",
		.group 		= IO_GROUP_A,
		.u8_number 	= 0,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA01] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 1,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA02] =
	{
		.kpc_name 	= "CHANNEL_SWITCH",
		.group 		= IO_GROUP_A,
		.u8_number 	= 2,
		.type		= IO_PIN_TYPE_INPUT
	},
	[IO_PIN_ID_PA03] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 3,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA04] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 4,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA05] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 5,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA06] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 6,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA07] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 7,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA08] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 8,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA09] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 9,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA10] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 10,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA11] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 11,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA12] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 12,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA13] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 13,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA14] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 14,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA15] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 15,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA16] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 16,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA17] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 17,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA18] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 18,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA19] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 19,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA20] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 20,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA21] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 21,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA22] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 22,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA23] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 23,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA24] =
	{
		.kpc_name 	= "UART_CHANNEL_SHELL_TX",
		.group 		= IO_GROUP_A,
		.u8_number 	= 24,
		.type		= IO_PIN_TYPE_PERIPHERAL
	},
	[IO_PIN_ID_PA25] =
	{
		.kpc_name 	= "UART_CHANNEL_SHELL_RX",
		.group 		= IO_GROUP_A,
		.u8_number 	= 25,
		.type		= IO_PIN_TYPE_PERIPHERAL
	},
	[IO_PIN_ID_PA27] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 27,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA28] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 28,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA30] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 30,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA31] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 31,
		.type		= IO_PIN_TYPE_UNUSED
	},

	// Port B

	[IO_PIN_ID_PB00] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 0,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB01] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 1,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB02] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 2,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB03] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 3,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB04] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 4,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB05] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 5,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB06] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 6,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB07] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 7,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB08] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 8,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB09] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 9,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB10] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 10,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB11] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 11,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB12] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 12,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB13] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 13,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB14] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 14,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB15] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 15,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB16] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 16,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB17] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 17,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB22] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 22,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB23] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 23,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB30] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 30,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB31] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 31,
		.type		= IO_PIN_TYPE_UNUSED
	}
};
#endif // AUDIO_SWITCH

#ifdef JLINK_MUX
static IO_pin_t p_pin_map[IO_PIN_ID_NUM_PINS] =
{
	// Port A

	[IO_PIN_ID_PA00] =
	{
		.kpc_name 	= "PORT_7_LED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 0,
		.type		= IO_PIN_TYPE_OUTPUT
	},
	[IO_PIN_ID_PA01] =
	{
		.kpc_name 	= "PORT_6_LED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 1,
		.type		= IO_PIN_TYPE_OUTPUT
	},
	[IO_PIN_ID_PA02] =
	{
		.kpc_name 	= "PORT_1_LED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 2,
		.type		= IO_PIN_TYPE_OUTPUT
	},
	[IO_PIN_ID_PA03] =
	{
		.kpc_name 	= "PORT_0_LED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 3,
		.type		= IO_PIN_TYPE_OUTPUT
	},
	[IO_PIN_ID_PA04] =
	{
		.kpc_name 	= "nPORT_3_SELECT",
		.group 		= IO_GROUP_A,
		.u8_number 	= 4,
		.type		= IO_PIN_TYPE_OUTPUT
	},
	[IO_PIN_ID_PA05] =
	{
		.kpc_name 	= "nPORT_2_SELECT",
		.group 		= IO_GROUP_A,
		.u8_number 	= 5,
		.type		= IO_PIN_TYPE_OUTPUT
	},
	[IO_PIN_ID_PA06] =
	{
		.kpc_name 	= "nPORT_1_SELECT",
		.group 		= IO_GROUP_A,
		.u8_number 	= 6,
		.type		= IO_PIN_TYPE_OUTPUT
	},
	[IO_PIN_ID_PA07] =
	{
		.kpc_name 	= "nPORT_0_SELECT",
		.group 		= IO_GROUP_A,
		.u8_number 	= 7,
		.type		= IO_PIN_TYPE_OUTPUT
	},
	[IO_PIN_ID_PA08] =
	{
		.kpc_name 	= "PORT_0_BUTTON",
		.group 		= IO_GROUP_A,
		.u8_number 	= 8,
		.type		= IO_PIN_TYPE_INPUT
	},
	[IO_PIN_ID_PA09] =
	{
		.kpc_name 	= "PORT_1_BUTTON",
		.group 		= IO_GROUP_A,
		.u8_number 	= 9,
		.type		= IO_PIN_TYPE_INPUT
	},
	[IO_PIN_ID_PA10] =
	{
		.kpc_name 	= "PORT_2_BUTTON",
		.group 		= IO_GROUP_A,
		.u8_number 	= 10,
		.type		= IO_PIN_TYPE_INPUT
	},
	[IO_PIN_ID_PA11] =
	{
		.kpc_name 	= "PORT_3_BUTTON",
		.group 		= IO_GROUP_A,
		.u8_number 	= 11,
		.type		= IO_PIN_TYPE_INPUT
	},
	[IO_PIN_ID_PA12] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 12,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA13] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 13,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA14] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 14,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA15] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 15,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA16] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 16,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA17] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 17,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA18] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 18,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA19] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 19,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA20] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 20,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA21] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 21,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA22] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 22,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA23] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 23,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA24] =
	{
		.kpc_name 	= "UART_CHANNEL_SHELL_TX",
		.group 		= IO_GROUP_A,
		.u8_number 	= 24,
		.type		= IO_PIN_TYPE_PERIPHERAL
	},
	[IO_PIN_ID_PA25] =
	{
		.kpc_name 	= "UART_CHANNEL_SHELL_RX",
		.group 		= IO_GROUP_A,
		.u8_number 	= 25,
		.type		= IO_PIN_TYPE_PERIPHERAL
	},
	[IO_PIN_ID_PA27] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 27,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA28] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 28,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA30] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 30,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PA31] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_A,
		.u8_number 	= 31,
		.type		= IO_PIN_TYPE_UNUSED
	},

	// Port B

	[IO_PIN_ID_PB00] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 0,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB01] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 1,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB02] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 2,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB03] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 3,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB04] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 4,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB05] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 5,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB06] =
	{
		.kpc_name 	= "nPORT_7_SELECT",
		.group 		= IO_GROUP_B,
		.u8_number 	= 6,
		.type		= IO_PIN_TYPE_OUTPUT
	},
	[IO_PIN_ID_PB07] =
	{
		.kpc_name 	= "nPORT_6_SELECT",
		.group 		= IO_GROUP_B,
		.u8_number 	= 7,
		.type		= IO_PIN_TYPE_OUTPUT
	},
	[IO_PIN_ID_PB08] =
	{
		.kpc_name 	= "nPORT_5_SELECT",
		.group 		= IO_GROUP_B,
		.u8_number 	= 8,
		.type		= IO_PIN_TYPE_OUTPUT
	},
	[IO_PIN_ID_PB09] =
	{
		.kpc_name 	= "nPORT_4_SELECT",
		.group 		= IO_GROUP_B,
		.u8_number 	= 9,
		.type		= IO_PIN_TYPE_OUTPUT
	},
	[IO_PIN_ID_PB10] =
	{
		.kpc_name 	= "PORT_4_BUTTON",
		.group 		= IO_GROUP_B,
		.u8_number 	= 10,
		.type		= IO_PIN_TYPE_INPUT
	},
	[IO_PIN_ID_PB11] =
	{
		.kpc_name 	= "PORT_5_BUTTON",
		.group 		= IO_GROUP_B,
		.u8_number 	= 11,
		.type		= IO_PIN_TYPE_INPUT
	},
	[IO_PIN_ID_PB12] =
	{
		.kpc_name 	= "PORT_6_BUTTON",
		.group 		= IO_GROUP_B,
		.u8_number 	= 12,
		.type		= IO_PIN_TYPE_INPUT
	},
	[IO_PIN_ID_PB13] =
	{
		.kpc_name 	= "PORT_7_BUTTON",
		.group 		= IO_GROUP_B,
		.u8_number 	= 13,
		.type		= IO_PIN_TYPE_INPUT
	},
	[IO_PIN_ID_PB14] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 14,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB15] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 15,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB16] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 16,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB17] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 17,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB22] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 22,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB23] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 23,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB30] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 30,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PB31] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_B,
		.u8_number 	= 31,
		.type		= IO_PIN_TYPE_UNUSED
	},

	// Port C

	[IO_PIN_ID_PC00] =
	{
		.kpc_name 	= "PORT_5_LED",
		.group 		= IO_GROUP_C,
		.u8_number 	= 0,
		.type		= IO_PIN_TYPE_OUTPUT
	},
	[IO_PIN_ID_PC01] =
	{
		.kpc_name 	= "PORT_4_LED",
		.group 		= IO_GROUP_C,
		.u8_number 	= 1,
		.type		= IO_PIN_TYPE_OUTPUT
	},
	[IO_PIN_ID_PC02] =
	{
		.kpc_name 	= "PORT_3_LED",
		.group 		= IO_GROUP_C,
		.u8_number 	= 2,
		.type		= IO_PIN_TYPE_OUTPUT
	},
	[IO_PIN_ID_PC03] =
	{
		.kpc_name 	= "PORT_2_LED",
		.group 		= IO_GROUP_C,
		.u8_number 	= 3,
		.type		= IO_PIN_TYPE_OUTPUT
	},
	[IO_PIN_ID_PC05] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_C,
		.u8_number 	= 5,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PC06] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_C,
		.u8_number 	= 6,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PC07] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_C,
		.u8_number 	= 7,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PC08] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_C,
		.u8_number 	= 8,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PC09] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_C,
		.u8_number 	= 9,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PC10] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_C,
		.u8_number 	= 10,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PC11] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_C,
		.u8_number 	= 11,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PC12] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_C,
		.u8_number 	= 12,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PC13] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_C,
		.u8_number 	= 13,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PC14] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_C,
		.u8_number 	= 14,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PC15] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_C,
		.u8_number 	= 15,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PC16] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_C,
		.u8_number 	= 16,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PC17] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_C,
		.u8_number 	= 17,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PC18] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_C,
		.u8_number 	= 18,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PC19] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_C,
		.u8_number 	= 19,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PC20] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_C,
		.u8_number 	= 20,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PC21] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_C,
		.u8_number 	= 21,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PC24] =
	{
		.kpc_name 	= "UART_CHANNEL_SHELL_TX",
		.group 		= IO_GROUP_C,
		.u8_number 	= 24,
		.type		= IO_PIN_TYPE_PERIPHERAL
	},
	[IO_PIN_ID_PC25] =
	{
		.kpc_name 	= "UART_CHANNEL_SHELL_RX",
		.group 		= IO_GROUP_C,
		.u8_number 	= 25,
		.type		= IO_PIN_TYPE_PERIPHERAL
	},
	[IO_PIN_ID_PC27] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_C,
		.u8_number 	= 27,
		.type		= IO_PIN_TYPE_UNUSED
	},
	[IO_PIN_ID_PC28] =
	{
		.kpc_name 	= "UNUSED",
		.group 		= IO_GROUP_C,
		.u8_number 	= 28,
		.type		= IO_PIN_TYPE_UNUSED
	},
};
#endif // JLINK_MUX

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	Initializes the IO Module
 *
 ****************************************************************************************************/
void IO_init(void)
{
	char * 	pc_string;
	char 	c_port;

	// Set the APB for the PORT peripheral
	MCLK_REGS->MCLK_APBBMASK |= MCLK_APBBMASK_PORT(1);

	for (uint8_t i = 0; i < IO_PIN_ID_NUM_PINS; i++)
	{
		if (!p_pin_map[i].kpc_name)
		{
			continue;
		}
		
		switch (p_pin_map[i].group)
		{
			case IO_GROUP_A:
			{
				c_port = 'A';
				break;
			}
			case IO_GROUP_B:
			{
				c_port = 'B';
				break;
			}
			case IO_GROUP_C:
			{
				c_port = 'C';
				break;
			}
			default:
				c_port = 'X';
		}

		pc_string = p_pin_map[i].pc_string;

		memset(pc_string, 0, IO_PIN_STRING_DESCRIPTOR_SIZE);
		sprintf(pc_string, "P%c%02u", c_port, p_pin_map[i].u8_number);

		// If the pin is an input or output, configure it here
		if (IO_PIN_TYPE_INPUT == p_pin_map[i].type)
		{
			IO_config_pin_direction(i, IO_DIRECTION_INPUT);
		}
		else if (IO_PIN_TYPE_OUTPUT == p_pin_map[i].type)
		{
			IO_config_pin_direction(i, IO_DIRECTION_OUTPUT);
		}
	}
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

	IO_pin_t pin = p_pin_map[pin_id];
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

	IO_pin_t pin = p_pin_map[pin_id];
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

    IO_pin_t pin = p_pin_map[pin_id];
    port_group_registers_t * group = &PORT_REGS->GROUP[pin.group];

    if (direction == IO_DIRECTION_OUTPUT)
    {
        group->PORT_DIRSET = (1u << pin.u8_number);
        group->PORT_PINCFG[pin.u8_number] = PORT_PINCFG_INEN(0);
    }
    else
    {
        group->PORT_DIRCLR = (1u << pin.u8_number);
        group->PORT_PINCFG[pin.u8_number] = PORT_PINCFG_INEN(1);
    }
}

// void IO_config_pin_direction(IO_pin_id_t pin_id, IO_pin_direction_t direction)
// {
// 	ASSERT(pin_id < IO_PIN_ID_NUM_PINS);
// 	ASSERT(direction < IO_DIRECTION_NUM_DIRECTIONS);

// 	IO_pin_t pin = p_pin_map[pin_id];

// 	PORT_REGS->GROUP[pin.group].PORT_DIR |= direction << pin.u8_number;
// }

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
	IO_pin_t pin = p_pin_map[pin_id];
	const uint32_t ku32_mask = 1U << pin.u8_number;
	port_group_registers_t * p_group = &PORT_REGS->GROUP[pin.group];

	if (state == IO_PIN_STATE_HIGH)
	{
		p_group->PORT_OUTSET = ku32_mask;
	}
	else
	{
		p_group->PORT_OUTCLR = ku32_mask;
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
	IO_pin_t pin = p_pin_map[pin_id];

	return (IO_pin_state_t)((PORT_REGS->GROUP[pin.group].PORT_IN >> pin.u8_number) & 1);
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

	IO_pin_t pin = p_pin_map[pin_id];

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

	IO_pin_t pin = p_pin_map[pin_id];

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

	IO_pin_t pin = p_pin_map[pin_id];

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

	IO_pin_t pin = p_pin_map[pin_id];

	PORT_REGS->GROUP[pin.group].PORT_PINCFG[pin.u8_number] &= ~PORT_PINCFG_DRVSTR(1);
}

/****************************************************************************************************
 *	Retrieves the name of the given pin according to its entry in the pin map
 *  
 *	@param[in] pin_id The ID of the desired pin
 *
 * 	@return The name of the pin
 * 
 ****************************************************************************************************/
const char * IO_get_pin_name(IO_pin_id_t pin_id)
{
	ASSERT(pin_id < IO_PIN_ID_NUM_PINS);

	return p_pin_map[pin_id].kpc_name;
}

/****************************************************************************************************
 *	Retrieves the string descriptor of the given pin according to its entry in the pin map
 *  
 *	@param[in] pin_id The ID of the desired pin
 *
 * 	@return The name of the pin
 * 
 ****************************************************************************************************/
const char * IO_get_pin_string(IO_pin_id_t pin_id)
{
	ASSERT(pin_id < IO_PIN_ID_NUM_PINS);

	return p_pin_map[pin_id].pc_string;
}

/****************************************************************************************************
 *	Shell utility
 *
 * 	Displays the application pin map
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t	IO_shell_map(uint8_t argc, char ** argv)
{
	if (argc == 0)
	{
		for (uint32_t i = 0; i < IO_PIN_ID_NUM_PINS; i++)
		{
			if (p_pin_map[i].kpc_name)
			{
				SHELL_printf("%u\t%s\t%s\n", i + 1, p_pin_map[i].pc_string, p_pin_map[i].kpc_name);
			}
		}
	}
	else
	{
		SHELL_printf("Usage: io map\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

/****************************************************************************************************
 *	Shell utility
 *
 * 	Sets a pin high or low
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t	IO_shell_set(uint8_t argc, char ** argv)
{
	uint32_t 		u32_high_or_low;
	IO_pin_id_t 	pin_id;
	char			pc_pin_name[IO_PIN_STRING_DESCRIPTOR_SIZE] = { 0 };
	bool 			b_res = true;

	if (argc == 2)
	{
		strncpy(pc_pin_name, argv[0], IO_PIN_STRING_DESCRIPTOR_SIZE);

		if (b_res && UTILS_string_to_u32(argv[1], &u32_high_or_low))
		{
			if (u32_high_or_low == 0 || u32_high_or_low == 1)
			{
				b_res = true;
			}
		}
	}

	if (b_res)
	{
		b_res = false;

		for (uint32_t i = 0; i < IO_PIN_ID_NUM_PINS; i++)
		{
			if (0 == strncmp(pc_pin_name, p_pin_map[i].pc_string, IO_PIN_STRING_DESCRIPTOR_SIZE))
			{
				pin_id = (IO_pin_id_t)i;
				b_res = true;
				break;
			}
		}
	}

	if (b_res)
	{
		IO_config_pin_direction(pin_id, IO_DIRECTION_OUTPUT);
		IO_set_pin(pin_id, u32_high_or_low);
		SHELL_printf("Set pin %s %s\n", pc_pin_name, u32_high_or_low == 1 ? "high": "low");
	}
	else
	{
		SHELL_printf("Usage: io set <pin number> <0|1>\n");
	}

	return SHELL_COMMAND_SUCCESS;
}
