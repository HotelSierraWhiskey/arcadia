#ifndef BUTTON_H
#define BUTTON_H

#include "common.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

typedef enum
{
	BUTTON_ID_A = 0,
	BUTTON_ID_B,
	BUTTON_ID_DPAD_UP,
	BUTTON_ID_DPAD_DOWN,
	BUTTON_ID_DPAD_LEFT,
	BUTTON_ID_DPAD_RIGHT,
	BUTTON_ID_MENU,
	//////////
	BUTTON_ID_NUM_BUTTONS
} BUTTON_id_t;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void 	BUTTON_update_buttons	(void);
bool 	BUTTON_is_pressed		(BUTTON_id_t button);

#endif // BUTTON_H
