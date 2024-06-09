#include "button.h"
#include "io.h"


#define BUTTON_PRESSED		0xFFFFFFFF
#define BUTTON_RELEASED 	0x00000000


typedef struct _button_map
{
	uint32_t 				u32_history;
	IO_pin_designation_t 	pin;
} BUTTON_map_entry_t;

static BUTTON_map_entry_t BUTTON_map[BUTTON_ID_NUM_BUTTONS] =
{
	[BUTTON_ID_DEBUG] =
	{
		.u32_history = BUTTON_RELEASED,
		.pin = IO_PIN_PA01
	},
	[BUTTON_ID_BOOT] =
	{
		.u32_history = BUTTON_RELEASED,
		.pin = IO_PIN_PA00
	}
};

static void BUTTON_read(const BUTTON_id_t button)
{
	const IO_pin_designation_t pin = BUTTON_map[button].pin;

	BUTTON_map[button].u32_history <<= 1;

	if (!IO_pin_read(pin))
	{
		BUTTON_map[button].u32_history |= 1;
	}
}

void BUTTON_update_buttons(void)
{
	for (uint8_t u8_count = 0; u8_count < BUTTON_ID_NUM_BUTTONS; u8_count++)
	{
		BUTTON_read(u8_count);
	}
}

bool BUTTON_is_pressed(const BUTTON_id_t button)
{
	return BUTTON_map[button].u32_history == BUTTON_PRESSED;
}