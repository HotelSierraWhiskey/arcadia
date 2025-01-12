#ifndef MENU_FSM_H
#define MENU_FSM_H

#include "fsm_event.h"

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void 		MENU_FSM_init					(void);
void 		MENU_FSM_handle_menu_event		(FSM_EVENT_t event);

#endif // MENU_FSM_H
