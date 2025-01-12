#include "app_fsm.h"
#include "shell.h"
#include "menu_fsm.h"
#include "story_fsm.h"
#include "utils.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define APP_FSM_LOG_DBG(fmt, ...)   		SHELL_printf("\r%-12s" fmt, "[APP_FSM]", ##__VA_ARGS__)
#define APP_FSM_LOG_WARN(fmt, ...)   		SHELL_PRINT_WARNING("\r%-12s" fmt, "[APP_FSM]", ##__VA_ARGS__)

typedef void (* APP_FSM_event_handler_t)(FSM_EVENT_t);

typedef struct _APP_FSM
{
	const char * 				kpc_name;
	APP_FSM_event_handler_t 	event_handler;
} APP_FSM_t;

typedef struct _APP_FSM_info
{
	APP_FSM_t * p_fsm;
} APP_FSM_info_t;

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

// None atm

/****************************************************************************************************
 *	V A R I A B L E S
 ****************************************************************************************************/

static const char * const kpc_event_descriptors[FSM_EVENT_NUM_EVENTS] =
{
	[FSM_EVENT_BUTTON_UP_PRESSED] 		= "EVENT_BUTTON_UP_PRESSED",
	[FSM_EVENT_BUTTON_DOWN_PRESSED] 	= "EVENT_BUTTON_DOWN_PRESSED",
	[FSM_EVENT_BUTTON_RIGHT_PRESSED] 	= "EVENT_BUTTON_RIGHT_PRESSED",
	[FSM_EVENT_BUTTON_LEFT_PRESSED] 	= "EVENT_BUTTON_LEFT_PRESSED",
	[FSM_EVENT_BUTTON_A_PRESSED]		= "EVENT_BUTTON_A_PRESSED",
	[FSM_EVENT_BUTTON_B_PRESSED]		= "EVENT_BUTTON_B_PRESSED",
	[FSM_EVENT_BUTTON_MENU_PRESSED]		= "EVENT_BUTTON_MENU_PRESSED",
};

static APP_FSM_t p_fsms[APP_FSM_ID_NUM_FSM_IDS] = 
{
	[APP_FSM_ID_MENU] = 
	{
		.kpc_name 		= "MENU_FSM",
		.event_handler 	= MENU_FSM_handle_menu_event,
	},
	[APP_FSM_ID_STORY] =
	{
		.kpc_name 		= "STORY_FSM",
		.event_handler 	= STORY_FSM_handle_story_event,
	}
};

static APP_FSM_info_t APP_FSM_info;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void APP_FSM_init(void)
{
	MENU_FSM_init();
	STORY_FSM_init();

	APP_FSM_info.p_fsm = &p_fsms[APP_FSM_ID_MENU];
}

void APP_FSM_handle_event(FSM_EVENT_t event)
{
	ASSERT(event < FSM_EVENT_NUM_EVENTS);
	APP_FSM_LOG_DBG("%s\n", kpc_event_descriptors[event]);
	APP_FSM_info.p_fsm->event_handler(event);
}

void APP_FSM_switch_to_fsm(APP_FSM_id_t fsm_id)
{
	ASSERT(fsm_id < APP_FSM_ID_NUM_FSM_IDS);
	APP_FSM_info.p_fsm = &p_fsms[fsm_id];
}
