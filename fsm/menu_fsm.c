#include "menu_fsm.h"
#include "app_fsm.h"
#include "shell.h"
#include "utils.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define MENU_FSM_LOG_DBG(fmt, ...)   		SHELL_printf("\r%-12s" fmt, "[MENU_FSM]", ##__VA_ARGS__)
#define MENU_FSM_LOG_WARN(fmt, ...)   		SHELL_PRINT_WARNING("\r%-12s" fmt, "[MENU_FSM]", ##__VA_ARGS__)

#define MENU_FSM_VARIABLE_NUM_SUB_MENUS		(-1)

typedef enum _MENU_FSM_main_menu_sub_menu_id
{
	MENU_FSM_MAIN_MENU_SUB_MENU_ID_STORIES = 0,
	MENU_FSM_MAIN_MENU_SUB_MENU_ID_SETTINGS,
	//////////
	MENU_FSM_MAIN_MENU_NUM_SUB_MENU_IDS
} MENU_FSM_main_menu_sub_menu_id_t;

typedef struct _MENU_FSM_menu
{
	const char * 				kpc_name;
	int8_t						i8_num_sub_menus;
	struct _MENU_FSM_menu **	pp_sub_menus;
	struct _MENU_FSM_menu *		p_highlighted_sub_menu;
	struct _MENU_FSM_menu *		p_next_menu;
	struct _MENU_FSM_menu *		p_prev_menu;
	struct _MENU_FSM_menu *		p_parent_menu;
} MENU_FSM_menu_t;

typedef struct _MENU_FSM_state_
{
	MENU_FSM_menu_t * p_current_menu;
} MENU_FSM_state_t;

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void 	MENU_FSM_go_to_next_menu				(void);
static void 	MENU_FSM_go_to_prev_menu				(void);
static void 	MENU_FSM_go_to_parent_menu				(void);

static void 	MENU_FSM_main_menu_stories_selected		(void);
static void 	MENU_FSM_main_menu_settings_selected	(void);

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

static MENU_FSM_menu_t main_menu;
static MENU_FSM_menu_t stories_menu;
static MENU_FSM_menu_t settings_menu;

/**
 *	Stories Menu struct
 */
static MENU_FSM_menu_t stories_menu =
{
	.kpc_name 				= "STORIES_MENU",
	.i8_num_sub_menus 		= MENU_FSM_VARIABLE_NUM_SUB_MENUS,
	.pp_sub_menus			= NULL,
	.p_highlighted_sub_menu = NULL,
	.p_next_menu			= &settings_menu,
	.p_prev_menu			= &stories_menu,
	.p_parent_menu 			= &main_menu,
};

/**
 *	Settings Menu struct
 */
static MENU_FSM_menu_t settings_menu =
{
	.kpc_name 				= "SETTINGS_MENU",
	.i8_num_sub_menus 		= 0,
	.pp_sub_menus			= NULL,
	.p_highlighted_sub_menu = NULL,
	.p_next_menu			= &settings_menu,
	.p_prev_menu			= &stories_menu,
	.p_parent_menu 			= &main_menu
};

/**
 *	Main Menu sub-menus
 */
static const MENU_FSM_menu_t * const pp_main_menu_sub_menus[MENU_FSM_MAIN_MENU_NUM_SUB_MENU_IDS] =
{
	[MENU_FSM_MAIN_MENU_SUB_MENU_ID_STORIES] 	= &stories_menu,
	[MENU_FSM_MAIN_MENU_SUB_MENU_ID_SETTINGS]	= &settings_menu
};

/**
 *	Main Menu struct
 */
static MENU_FSM_menu_t main_menu =
{
	.kpc_name 				= "MAIN_MENU",
	.i8_num_sub_menus		= MENU_FSM_MAIN_MENU_NUM_SUB_MENU_IDS,
	.pp_sub_menus			= pp_main_menu_sub_menus,
	.p_highlighted_sub_menu	= &stories_menu,
	.p_next_menu			= NULL,
	.p_prev_menu			= NULL,
	.p_parent_menu			= &main_menu
};

/**
 *	Main Menu FSM state struct
 */
MENU_FSM_state_t MENU_FSM_state;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void MENU_FSM_init(void)
{
	MENU_FSM_state.p_current_menu = &main_menu;
}

void MENU_FSM_handle_menu_event(FSM_EVENT_t event)
{
	switch (event)
	{
		case FSM_EVENT_BUTTON_UP_PRESSED:
		{
			MENU_FSM_go_to_prev_menu();
			break;
		}
		case FSM_EVENT_BUTTON_DOWN_PRESSED:
		{
			MENU_FSM_go_to_next_menu();
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
			MENU_FSM_go_to_parent_menu();
			break;
		}
		case FSM_EVENT_BUTTON_MENU_PRESSED:
		{
			// Only if there's a story being processed
			APP_FSM_switch_to_fsm(APP_FSM_ID_STORY);
			break;
		}
	}
}

static void MENU_FSM_go_to_next_menu(void)
{
	MENU_FSM_menu_t * p_menu = MENU_FSM_state.p_current_menu;

	p_menu->p_highlighted_sub_menu = p_menu->p_highlighted_sub_menu->p_next_menu;

	MENU_FSM_LOG_DBG("Current Menu: %s, Highlighted Sub-menu: %s\n", 
		MENU_FSM_state.p_current_menu->kpc_name, p_menu->p_highlighted_sub_menu->kpc_name);
}

static void MENU_FSM_go_to_prev_menu(void)
{
	MENU_FSM_menu_t * p_menu = MENU_FSM_state.p_current_menu;

	p_menu->p_highlighted_sub_menu = p_menu->p_highlighted_sub_menu->p_prev_menu;

	MENU_FSM_LOG_DBG("Current Menu: %s, Highlighted Sub-menu: %s\n", 
		MENU_FSM_state.p_current_menu->kpc_name, p_menu->p_highlighted_sub_menu->kpc_name);
}

static void MENU_FSM_go_to_parent_menu(void)
{
	MENU_FSM_menu_t * p_menu = MENU_FSM_state.p_current_menu;

	p_menu = p_menu->p_parent_menu;

	MENU_FSM_LOG_DBG("Current Menu: %s, Highlighted Sub-menu: %s\n", 
		MENU_FSM_state.p_current_menu->kpc_name, p_menu->p_highlighted_sub_menu->kpc_name);
}

static void MENU_FSM_main_menu_stories_selected(void)
{
	MENU_FSM_LOG_DBG("Stories Menu Selected\n");
}

static void MENU_FSM_main_menu_settings_selected(void)
{
	MENU_FSM_LOG_DBG("Settings Menu Selected\n");
}
