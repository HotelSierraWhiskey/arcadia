#include "button.h"
#include "io.h"
#include "shell.h"
#include "utils.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define BUTTON_LOG_DBG(fmt, ...)   		SHELL_printf("\r%-12s" fmt, "[BUTTON]", ##__VA_ARGS__)
#define BUTTON_LOG_WARN(fmt, ...)   	SHELL_PRINT_WARNING("\r%-12s" fmt, "[BUTTON]", ##__VA_ARGS__)

#define BUTTON_PRESSED		0xFFFFFFFFFFFFFFFF
#define BUTTON_RELEASED 	0x0000000000000000

typedef struct _BUTTON_map_entry
{
	uint64_t 				u64_history;
	IO_pin_id_t			 	pin;
	BUTTON_callback_t		callback;
} BUTTON_entry_t;

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

static BUTTON_entry_t BUTTON_map[BUTTON_ID_NUM_BUTTONS] =
{
#ifdef JLINK_MUX
	[BUTTON_ID_PORT_0] =
	{
		.u64_history 	= BUTTON_RELEASED,
		.pin 			= IO_PIN_ID_PA08,
		.callback		= NULL
	},
	[BUTTON_ID_PORT_1] =
	{
		.u64_history 	= BUTTON_RELEASED,
		.pin 			= IO_PIN_ID_PA09,
		.callback		= NULL
	},
	[BUTTON_ID_PORT_2] =
	{
		.u64_history 	= BUTTON_RELEASED,
		.pin 			= IO_PIN_ID_PA10,
		.callback		= NULL
	},
	[BUTTON_ID_PORT_3] =
	{
		.u64_history 	= BUTTON_RELEASED,
		.pin 			= IO_PIN_ID_PA11,
		.callback		= NULL
	},
	[BUTTON_ID_PORT_4] =
	{
		.u64_history 	= BUTTON_RELEASED,
		.pin 			= IO_PIN_ID_PB10,
		.callback		= NULL
	},
	[BUTTON_ID_PORT_5] =
	{
		.u64_history 	= BUTTON_RELEASED,
		.pin 			= IO_PIN_ID_PB11,
		.callback		= NULL
	},
	[BUTTON_ID_PORT_6] =
	{
		.u64_history 	= BUTTON_RELEASED,
		.pin 			= IO_PIN_ID_PB12,
		.callback		= NULL
	},
	[BUTTON_ID_PORT_7] =
	{
		.u64_history 	= BUTTON_RELEASED,
		.pin 			= IO_PIN_ID_PB13,
		.callback		= NULL
	},
#endif // JLINK_MUX
};

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void 	BUTTON_read		(const BUTTON_id_t button);

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void BUTTON_update(void)
{
	for (uint8_t i = 0; i < BUTTON_ID_NUM_BUTTONS; i++)
	{
		BUTTON_read(i);

		if (BUTTON_is_pressed(i))
		{
			if (BUTTON_map[i].callback)
			{
				BUTTON_map[i].callback();
			}
		}
	}
}

bool BUTTON_is_pressed(const BUTTON_id_t button)
{
	return BUTTON_map[button].u64_history == BUTTON_PRESSED;
}

void BUTTON_register_callback(BUTTON_id_t button_id, BUTTON_callback_t callback)
{
	ASSERT(button_id < BUTTON_ID_NUM_BUTTONS);
	ASSERT(callback);

	BUTTON_map[button_id].callback = callback;
}

static void BUTTON_read(const BUTTON_id_t button)
{
	const IO_pin_id_t pin = BUTTON_map[button].pin;

	BUTTON_map[button].u64_history <<= 1;

	if (IO_PIN_STATE_LOW == IO_read_pin(pin))
	{
		BUTTON_map[button].u64_history |= 1;
	}
}
