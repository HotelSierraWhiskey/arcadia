#ifndef BUTTON_H
#define BUTTON_H

#include "common.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

typedef enum
{
#ifdef JLINK_MUX
	BUTTON_ID_PORT_0,
	BUTTON_ID_PORT_1,
	BUTTON_ID_PORT_2,
	BUTTON_ID_PORT_3,
	BUTTON_ID_PORT_4,
	BUTTON_ID_PORT_5,
	BUTTON_ID_PORT_6,
	BUTTON_ID_PORT_7,
	//////////
#endif // JLINK_MUX
	BUTTON_ID_NUM_BUTTONS
} BUTTON_id_t;

typedef void (* BUTTON_callback_t)(void);

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void 	BUTTON_register_callback	(BUTTON_id_t button_id, BUTTON_callback_t callback);
void 	BUTTON_update				(void);
bool 	BUTTON_is_pressed			(const BUTTON_id_t button);

#endif