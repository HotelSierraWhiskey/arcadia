#include "menu_fsm.h"
#include "app_fsm.h"
#include "shell.h"
#include "utils.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define MENU_FSM_LOG_DBG(fmt, ...)   		SHELL_printf("\r%-12s" fmt, "[MENU_FSM]", ##__VA_ARGS__)
#define MENU_FSM_LOG_WARN(fmt, ...)   		SHELL_PRINT_WARNING("\r%-12s" fmt, "[MENU_FSM]", ##__VA_ARGS__)

#define MENU_FSM_NUM_MAIN_MENU_LINKS		(2U)

typedef enum _MENU_FSM_menu_id
{
	// Main Menu links
	MENU_FSM_MENU_ID_MAIN_MENU = 0,
	//////////
	MENU_FSM_MENU_ID_NUM_MENUS
} MENU_FSM_menu_id_t;

typedef enum _MENU_FSM_link_id
{
	// Main Menu links
	MENU_FSM_link_ID_MAIN_MENU_STORIES,
	MENU_FSM_link_ID_MAIN_MENU_SETTINGS,
	//////////
	MENU_FSM_link_ID_NUM_LINKS
} MENU_FSM_link_id_t;

typedef void (* MENU_FSM_selection_callback_t)(void);

typedef struct _MENU_FSM_link
{
	MENU_FSM_link_id_t				id;
	const char * 					kpc_link_name;
	MENU_FSM_selection_callback_t	on_select;
	MENU_FSM_link_id_t				next_link_id;
	MENU_FSM_link_id_t				prev_link_id;
	MENU_FSM_menu_t *				p_menu;
} MENU_FSM_link_t;

typedef struct _MENU_FSM_menu
{
	const char * 			kpc_name;
	MENU_FSM_link_t * 		p_links;
	uint8_t					u8_num_links;
	MENU_FSM_link_t *		p_current_link;
} MENU_FSM_menu_t;

typedef struct _MENU_FSM_state_
{
	MENU_FSM_menu_t * p_current_menu;
} MENU_FSM_state_t;

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void 	MENU_FSM_next_link						(MENU_FSM_menu_t * p_menu);
static void 	MENU_FSM_prev_link						(MENU_FSM_menu_t * p_menu);

static void 	MENU_FSM_main_menu_stories_selected		(void);
static void 	MENU_FSM_main_menu_settings_selected	(void);

/****************************************************************************************************
 *	V A R I A B L E S
 ****************************************************************************************************/

/**
 *	Main Menu Stuff 
 */
const MENU_FSM_link_t k_stories_link =
{
	.id 				= MENU_FSM_link_ID_MAIN_MENU_STORIES,
	.kpc_link_name		= "STORIES",
	.on_select			= MENU_FSM_main_menu_stories_selected,
	.next_link_id		= MENU_FSM_link_ID_MAIN_MENU_SETTINGS,
	.prev_link_id		= MENU_FSM_link_ID_MAIN_MENU_STORIES
};

const MENU_FSM_link_t k_settings_link =
{
	.id 				= MENU_FSM_link_ID_MAIN_MENU_SETTINGS,
	.kpc_link_name		= "SETTINGS",
	.on_select			= MENU_FSM_main_menu_settings_selected,
	.next_link_id		= MENU_FSM_link_ID_MAIN_MENU_SETTINGS,
	.prev_link_id		= MENU_FSM_link_ID_MAIN_MENU_STORIES
};

MENU_FSM_link_t p_main_menu_links[MENU_FSM_NUM_MAIN_MENU_LINKS] =
{
	k_stories_link,
	k_settings_link
};

MENU_FSM_menu_t main_menu =
{
	.kpc_name 			= "MAIN_MENU",
	.u8_num_links 		= MENU_FSM_NUM_MAIN_MENU_LINKS,
	.p_links			= p_main_menu_links,
	.p_current_link		= (MENU_FSM_link_t *)&k_settings_link
};

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
			MENU_FSM_next_link(MENU_FSM_state.p_current_menu);
			break;
		}
		case FSM_EVENT_BUTTON_DOWN_PRESSED:
		{
			MENU_FSM_prev_link(MENU_FSM_state.p_current_menu);
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
			MENU_FSM_state.p_current_menu->p_current_link->on_select();
			break;
		}
		case FSM_EVENT_BUTTON_B_PRESSED:
		{
			break;
		}
		case FSM_EVENT_BUTTON_MENU_PRESSED:
		{
			APP_FSM_switch_to_fsm(APP_FSM_ID_STORY);
			break;
		}
	}
}

static void MENU_FSM_next_link(MENU_FSM_menu_t * p_menu)
{
	MENU_FSM_link_id_t next_link_id = p_menu->p_current_link->next_link_id;

	for (uint8_t i = 0; i < p_menu->u8_num_links; i++)
	{
		if (p_menu->p_links[i].id == next_link_id)
		{
			p_menu->p_current_link = &p_menu->p_links[i];

			MENU_FSM_LOG_DBG("Current Menu: %s, Highlighted Link: %s\n", 
				MENU_FSM_state.p_current_menu->kpc_name, p_menu->p_current_link->kpc_link_name);

			return;
		}
	}
	ASSERT(0);
}

static void MENU_FSM_prev_link(MENU_FSM_menu_t * p_menu)
{
	MENU_FSM_link_id_t prev_link_id = p_menu->p_current_link->prev_link_id;

	for (uint8_t i = 0; i < p_menu->u8_num_links; i++)
	{
		if (p_menu->p_links[i].id == prev_link_id)
		{
			p_menu->p_current_link = &p_menu->p_links[i];

			MENU_FSM_LOG_DBG("Current Menu: %s, Highlighted Link: %s\n", 
				MENU_FSM_state.p_current_menu->kpc_name, p_menu->p_current_link->kpc_link_name);

			return;
		}
	}
	ASSERT(0);
}

static void MENU_FSM_main_menu_stories_selected(void)
{
	MENU_FSM_LOG_DBG("Stories Menu Selected\n");
}

static void MENU_FSM_main_menu_settings_selected(void)
{
	MENU_FSM_LOG_DBG("Settings Menu Selected\n");
}
