#include "story_fsm.h"
#include "app_fsm.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

typedef struct _STORY_FSM_state_
{

} STORY_FSM_state_t;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void STORY_FSM_init(void)
{
	
}

void STORY_FSM_handle_story_event(FSM_EVENT_t event)
{
	switch (event)
	{
		case FSM_EVENT_BUTTON_UP_PRESSED:
		{
			break;
		}
		case FSM_EVENT_BUTTON_DOWN_PRESSED:
		{
			break;
		}
		case FSM_EVENT_BUTTON_RIGHT_PRESSED:
		{
			break;
		}
		case FSM_EVENT_BUTTON_LEFT_PRESSED:
		{
			break;
		}
		case FSM_EVENT_BUTTON_A_PRESSED:
		{
			break;
		}
		case FSM_EVENT_BUTTON_B_PRESSED:
		{
			break;
		}
		case FSM_EVENT_BUTTON_MENU_PRESSED:
		{
			APP_FSM_switch_to_fsm(APP_FSM_ID_MENU);
			break;
		}
	}
}
