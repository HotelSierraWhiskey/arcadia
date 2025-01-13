#include "menu_fsm.h"
#include "app_fsm.h"
#include "shell.h"
#include "utils.h"
#include "menu_component.h"
#include "drive_api.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define MENU_FSM_LOG_DBG(fmt, ...)   		SHELL_printf("\r%-12s" fmt, "[MENU_FSM]", ##__VA_ARGS__)
#define MENU_FSM_LOG_WARN(fmt, ...)   		SHELL_PRINT_WARNING("\r%-12s" fmt, "[MENU_FSM]", ##__VA_ARGS__)

#define MENU_FSM_NUM_ON_SCREEN_TITLES		(5U)

typedef enum _MENU_FSM_menu_id
{
	MENU_FSM_MENU_ID_MAIN_MENU = 0,
	MENU_FSM_MENU_ID_MAIN_STORIES,
	MENU_FSM_MENU_ID_MAIN_SETTINGS
} MENU_FSM_menu_id_t;
typedef enum _MENU_FSM_main_menu_sub_menu_id
{
	MENU_FSM_MAIN_MENU_SUB_MENU_ID_STORIES = 0,
	MENU_FSM_MAIN_MENU_SUB_MENU_ID_SETTINGS,
	//////////
	MENU_FSM_MAIN_MENU_NUM_SUB_MENU_IDS
} MENU_FSM_main_menu_sub_menu_id_t;

typedef struct _MENU_FSM_menu
{
	MENU_FSM_menu_id_t			id;
	const char * 				kpc_name;
	int8_t						i8_num_sub_menus;
	struct _MENU_FSM_menu **	pp_sub_menus;
	struct _MENU_FSM_menu *		p_highlighted_sub_menu;
	struct _MENU_FSM_menu *		p_next_menu;
	struct _MENU_FSM_menu *		p_prev_menu;
	struct _MENU_FSM_menu *		p_parent_menu;
	MENU_COMPONENT_t **			pp_components;
	uint8_t						u8_num_components;
} MENU_FSM_menu_t;

typedef struct _MENU_FSM_state_
{
	MENU_FSM_menu_t * p_current_menu;
	char ppc_on_screen_titles_buffer[MENU_FSM_NUM_ON_SCREEN_TITLES][COMMON_MAX_FNAME_SIZE];
} MENU_FSM_state_t;

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void 	MENU_FSM_go_to_next_menu				(void);
static void 	MENU_FSM_go_to_prev_menu				(void);
static void 	MENU_FSM_go_to_parent_menu				(void);
static void		MENU_FSM_step_into_sub_menu				(void);

static void 	MENU_FSM_on_stories_menu_selected		(void);

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

static MENU_FSM_menu_t main_menu;
static MENU_FSM_menu_t stories_menu;
static MENU_FSM_menu_t settings_menu;

/**
 *	Stories Menu components
 */
static MENU_COMPONENT_t p_on_screen_titles[MENU_FSM_NUM_ON_SCREEN_TITLES];

/**
 *	Stories Menu struct
 */
static MENU_FSM_menu_t stories_menu =
{
	.id						= MENU_FSM_MENU_ID_MAIN_STORIES,
	.kpc_name 				= "STORIES_MENU",
	.i8_num_sub_menus 		= 0,
	.pp_sub_menus			= NULL,
	.p_highlighted_sub_menu = NULL,
	.p_next_menu			= &settings_menu,
	.p_prev_menu			= &stories_menu,
	.p_parent_menu 			= &main_menu,
	.pp_components			= (MENU_COMPONENT_t **)&p_on_screen_titles,
	.u8_num_components		= MENU_FSM_NUM_ON_SCREEN_TITLES,
};

/**
 *	Settings Menu struct
 */
static MENU_FSM_menu_t settings_menu =
{
	.id						= MENU_FSM_MENU_ID_MAIN_SETTINGS,
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
	.id						= MENU_FSM_MENU_ID_MAIN_MENU,
	.kpc_name 				= "MAIN_MENU",
	.i8_num_sub_menus		= MENU_FSM_MAIN_MENU_NUM_SUB_MENU_IDS,
	.pp_sub_menus			= (MENU_FSM_menu_t **)pp_main_menu_sub_menus,
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
	// uint8_t u8_num_found;
	
	MENU_FSM_state.p_current_menu = &main_menu;

	// how many stories are there? load up to on-screen max into the stories menu components

	// stories_menu.p_components
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
			MENU_FSM_step_into_sub_menu();
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

static void MENU_FSM_step_into_sub_menu(void)
{
	MENU_FSM_state.p_current_menu = MENU_FSM_state.p_current_menu->p_highlighted_sub_menu;

	switch (MENU_FSM_state.p_current_menu->id)
	{
		case MENU_FSM_MENU_ID_MAIN_MENU:
		{
			break;
		}
		case MENU_FSM_MENU_ID_MAIN_STORIES:
		{
			MENU_FSM_on_stories_menu_selected();
			break;
		}
		case MENU_FSM_MENU_ID_MAIN_SETTINGS:
		{
			break;
		}
	}
}

static void MENU_FSM_on_stories_menu_selected(void)
{
	uint8_t 			u8_num_found;
	ARCADIA_status_t 	status = ARCADIA_STATUS_FAILED;
	char * 				ppc_buffer_pointers[MENU_FSM_NUM_ON_SCREEN_TITLES];

	for (uint8_t i = 0; i < MENU_FSM_NUM_ON_SCREEN_TITLES; i++)
	{
        ppc_buffer_pointers[i] = p_on_screen_titles[i].story.pc_title;
    }

	MENU_FSM_LOG_DBG("Stories Menu Selected\n");
	
	status = DRIVE_API_fetch_fnames(MENU_FSM_NUM_ON_SCREEN_TITLES, 0, ppc_buffer_pointers, &u8_num_found);

	if (ARCADIA_STATUS_OK == status)
	{

	}
	else
	{
		MENU_FSM_LOG_WARN("Unable to retrieve file names (status: %u)\n", status);
	}

	SHELL_printf("Num found: %u\n", u8_num_found);

	for (uint8_t i = 0; i < u8_num_found; i++)
	{
		SHELL_printf("pc_title: %s\n", p_on_screen_titles[i].story.pc_title);
	}
}
