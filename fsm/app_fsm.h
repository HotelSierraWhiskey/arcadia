#ifndef APP_FSM_H
#define APP_FSM_H

#include "common.h"
#include "fsm_event.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

typedef enum _APP_FSM_id
{
	APP_FSM_ID_MENU = 0,
	APP_FSM_ID_STORY,
	//////////
	APP_FSM_ID_NUM_FSM_IDS
} APP_FSM_id_t;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void 	APP_FSM_init			(void);
void	APP_FSM_handle_event	(FSM_EVENT_t event);
void	APP_FSM_switch_to_fsm	(APP_FSM_id_t fsm_id);

#endif // APP_FSM_H
