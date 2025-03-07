#include "button.h"
#include "io.h"
#include "utils.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define BUTTON_PRESSED		0xFFFFFFFF
#define BUTTON_RELEASED 	0x00000000

typedef struct _button_map
{
	uint32_t 			u32_history;
	IO_pin_id_t 		pin;
} BUTTON_map_entry_t;

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

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

void BUTTON_update_buttons(void)
{
	for (uint8_t u8_count = 0; u8_count < BUTTON_ID_NUM_BUTTONS; u8_count++)
	{
		BUTTON_read(u8_count);
	}
}

bool BUTTON_is_pressed(BUTTON_id_t button_id)
{
	return BUTTON_map[button_id].u32_history == BUTTON_PRESSED;
}

bool BUTTON_is_released(BUTTON_id_t button_id)
{
	return BUTTON_map[button_id].u32_history == BUTTON_RELEASED;
}
