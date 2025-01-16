#ifndef FSM_EVENT_H
#define FSM_EVENT_H

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

typedef enum _FSM_EVENT
{
	FSM_EVENT_BUTTON_UP_PRESSED = 0,
	FSM_EVENT_BUTTON_DOWN_PRESSED,
	FSM_EVENT_BUTTON_RIGHT_PRESSED,
	FSM_EVENT_BUTTON_LEFT_PRESSED,
	FSM_EVENT_BUTTON_A_PRESSED,
	FSM_EVENT_BUTTON_B_PRESSED,
	FSM_EVENT_BUTTON_MENU_PRESSED,
	//////////
	FSM_EVENT_NUM_EVENTS
} FSM_EVENT_t;

#endif // FSM_EVENT_H
