#include "button.h"
#include "io.h"
#include "utils.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define BUTTON_PRESSED		0xFFFFFFFF
#define BUTTON_RELEASED 	0x00000000

/**
 *	A button entry
 *
 * 	Contains information for tracking and updating a button's state
 */
typedef struct _button_map
{
	uint32_t 			u32_history;
	IO_pin_id_t 		pin;
} BUTTON_map_entry_t;

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

/**
 *	The module's registry of buttons
 */
static BUTTON_map_entry_t BUTTON_map[BUTTON_ID_NUM_BUTTONS] =
{
	[BUTTON_ID_A] =
	{
		.u32_history 	= BUTTON_RELEASED,
		.pin 			= IO_PIN_ID_PA08
	},
	[BUTTON_ID_B] =
	{
		.u32_history 	= BUTTON_RELEASED,
		.pin 			= IO_PIN_ID_PA09
	},
	[BUTTON_ID_DPAD_UP] =
	{
		.u32_history 	= BUTTON_RELEASED,
		.pin 			= IO_PIN_ID_PA10
	},
	[BUTTON_ID_DPAD_DOWN] =
	{
		.u32_history 	= BUTTON_RELEASED,
		.pin 			= IO_PIN_ID_PA11
	},
	[BUTTON_ID_DPAD_LEFT] =
	{
		.u32_history 	= BUTTON_RELEASED,
		.pin 			= IO_PIN_ID_PA12
	},
	[BUTTON_ID_DPAD_RIGHT] =
	{
		.u32_history 	= BUTTON_RELEASED,
		.pin 			= IO_PIN_ID_PA13
	},
	[BUTTON_ID_MENU] =
	{
		.u32_history 	= BUTTON_RELEASED,
		.pin 			= IO_PIN_ID_PA14
	},
};

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void 	BUTTON_read		(BUTTON_id_t button_id);

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	Read and update button state history
 *
 * 	Reads the current state of the specified button and updates its history bit-shift buffer.
 * 	This function ensures that button presses are tracked over time for debounce handling.
 *
 *	@param[in] button_id The ID of the button to read
 *
 *	@return None
 ****************************************************************************************************/
static void BUTTON_read(BUTTON_id_t button_id)
{
	ASSERT(button_id < BUTTON_ID_NUM_BUTTONS);

	IO_pin_id_t pin = BUTTON_map[button_id].pin;

	BUTTON_map[button_id].u32_history <<= 1;

	if (IO_PIN_STATE_LOW == IO_read_pin(pin))
	{
		BUTTON_map[button_id].u32_history |= 1;
	}
}

/****************************************************************************************************
 *	Update all button states
 *
 * 	Iterates through all buttons and updates their history by calling `BUTTON_read`.
 * 
 ****************************************************************************************************/
void BUTTON_update_buttons(void)
{
	for (uint8_t u8_count = 0; u8_count < BUTTON_ID_NUM_BUTTONS; u8_count++)
	{
		BUTTON_read(u8_count);
	}
}

/****************************************************************************************************
 *	Check if a button is pressed
 *
 * 	Determines whether the specified button has registered as pressed based on its history buffer.
 * 
 *	@param[in] button_id The ID of the button to check
 *
 *	@return `true` if the button is pressed, `false` otherwise
 ****************************************************************************************************/
bool BUTTON_is_pressed(BUTTON_id_t button_id)
{
	return BUTTON_map[button_id].u32_history == BUTTON_PRESSED;
}

/****************************************************************************************************
 *	Check if a button is released
 *
 * 	Determines whether the specified button has registered as released based on its history buffer.
 * 
 *	@param[in] button_id The ID of the button to check
 *
 *	@return `true` if the button is released, `false` otherwise
 ****************************************************************************************************/
bool BUTTON_is_released(BUTTON_id_t button_id)
{
	return BUTTON_map[button_id].u32_history == BUTTON_RELEASED;
}
