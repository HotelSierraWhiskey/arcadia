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
#define MENU_FSM_ARC_PROJECT_FILTER			"*.arc"

typedef enum _MENU_FSM_state
{
	MENU_FSM_STATE_WAIT_FOR_MENU_EVENT = 0,
	MENU_FSM_STATE_WAIT_FOR_COMPONENT_EVENT,
	//////////
	MENU_FSM_STATE_NUM_STATES
} MENU_FSM_state_t;

typedef enum _MENU_FSM_direction
{
	MENU_FSM_DIRECTION_NEXT = 0,
	MENU_FSM_DIRECTION_PREV,
	//////////
	MENU_FSM_DIRECTION_NUM_DIRECTIONS
} MENU_FSM_direction_t;

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
	// Menu- & sub-menu-specific members
	MENU_FSM_menu_id_t			id;
	const char * 				kpc_name;
	int8_t						i8_num_sub_menus;
	struct _MENU_FSM_menu *		p_sub_menus;
	uint8_t 					u8_highlighted_submenu_index;
	struct _MENU_FSM_menu *		p_parent_menu;

	// Component-specific members
	uint8_t						u8_num_components;
	MENU_COMPONENT_t *			p_components;
	uint8_t						u8_highlighted_component_index;
} MENU_FSM_menu_t;

typedef struct _MENU_FSM_info_
{
	MENU_FSM_state_t 	state;
	MENU_FSM_menu_t * 	p_current_menu;
} MENU_FSM_info_t;

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void 	MENU_FSM_go_to_menu						(MENU_FSM_direction_t direction);
static void 	MENU_FSM_go_to_parent_menu				(void);
static void		MENU_FSM_step_into_sub_menu				(void);

static void 	MENU_FSM_go_to_component				(MENU_FSM_direction_t direction);

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
static MENU_COMPONENT_t p_on_screen_titles_components[MENU_FSM_NUM_ON_SCREEN_TITLES];

/**
 *	Stories Menu struct
 */
static MENU_FSM_menu_t stories_menu =
{
	.id								= MENU_FSM_MENU_ID_MAIN_STORIES,
	.kpc_name 						= "STORIES_MENU",
	.i8_num_sub_menus 				= 0,
	.p_sub_menus					= NULL,
	.u8_highlighted_submenu_index 	= 0,
	.p_parent_menu 					= &main_menu,
	.u8_num_components				= MENU_FSM_NUM_ON_SCREEN_TITLES,
	.p_components					= p_on_screen_titles_components,
	.u8_highlighted_component_index = 0
};

/**
 *	Settings Menu struct
 */
static MENU_FSM_menu_t settings_menu =
{
	.id								= MENU_FSM_MENU_ID_MAIN_SETTINGS,
	.kpc_name 						= "SETTINGS_MENU",
	.i8_num_sub_menus 				= 0,
	.p_sub_menus					= NULL,
	.u8_highlighted_submenu_index 	= 0,
	.p_parent_menu 					= &main_menu
};

/**
 *	Main Menu sub-menus
 */
static MENU_FSM_menu_t p_main_menu_sub_menus[MENU_FSM_MAIN_MENU_NUM_SUB_MENU_IDS];

/**
 *	Main Menu struct
 */
static MENU_FSM_menu_t main_menu =
{
	.id								= MENU_FSM_MENU_ID_MAIN_MENU,
	.kpc_name 						= "MAIN_MENU",
	.i8_num_sub_menus				= MENU_FSM_MAIN_MENU_NUM_SUB_MENU_IDS,
	.p_sub_menus					= p_main_menu_sub_menus,
	.u8_highlighted_submenu_index 	= 0,
	.p_parent_menu					= &main_menu
};

/**
 *	Main Menu FSM state struct
 */
MENU_FSM_info_t MENU_FSM_info;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void MENU_FSM_init(void)
{
	// Initialize Main Menu's sub-menus
	p_main_menu_sub_menus[MENU_FSM_MAIN_MENU_SUB_MENU_ID_STORIES] = stories_menu;
	p_main_menu_sub_menus[MENU_FSM_MAIN_MENU_SUB_MENU_ID_SETTINGS] = settings_menu;

	MENU_FSM_info.p_current_menu = &main_menu;
	MENU_FSM_info.state = MENU_FSM_STATE_WAIT_FOR_MENU_EVENT;
}

void MENU_FSM_handle_event(FSM_EVENT_t event)
{
	switch (MENU_FSM_info.state)
	{
		case MENU_FSM_STATE_WAIT_FOR_MENU_EVENT:
		{
			switch (event)
			{
				case FSM_EVENT_BUTTON_UP_PRESSED:
				{
					MENU_FSM_go_to_menu(MENU_FSM_DIRECTION_PREV);
					break;
				}
				case FSM_EVENT_BUTTON_DOWN_PRESSED:
				{
					MENU_FSM_go_to_menu(MENU_FSM_DIRECTION_NEXT);
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
		break;

		case MENU_FSM_STATE_WAIT_FOR_COMPONENT_EVENT:
		{
			switch (event)
			{
				case FSM_EVENT_BUTTON_UP_PRESSED:
				{
					MENU_FSM_go_to_component(MENU_FSM_DIRECTION_PREV);
					break;
				}
				case FSM_EVENT_BUTTON_DOWN_PRESSED:
				{
					MENU_FSM_go_to_component(MENU_FSM_DIRECTION_NEXT);
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
					break;
				}
			}
		}
		break;
	}
}

/****************************************************************************************************
 *	M E N U   N A V I G A T I O N
 ****************************************************************************************************/

static void MENU_FSM_go_to_menu(MENU_FSM_direction_t direction)
{
	MENU_FSM_menu_t * p_menu = MENU_FSM_info.p_current_menu;

	if (MENU_FSM_DIRECTION_NEXT == direction && 
		p_menu->u8_highlighted_submenu_index < (p_menu->i8_num_sub_menus - 1))
	{
		p_menu->u8_highlighted_submenu_index++;
	}
	else if (MENU_FSM_DIRECTION_PREV == direction && 
			p_menu->u8_highlighted_submenu_index > 0)
	{
		p_menu->u8_highlighted_submenu_index--;
	}

	MENU_FSM_LOG_DBG("Current Menu: %s, Highlighted Sub-menu: %s\n", 
		p_menu->kpc_name, p_menu->p_sub_menus[p_menu->u8_highlighted_submenu_index].kpc_name);
}

static void MENU_FSM_go_to_parent_menu(void)
{
	MENU_FSM_menu_t * p_menu = MENU_FSM_info.p_current_menu;

	// Restore the highlighted component
	if (p_menu->p_components)
	{
		MENU_FSM_info.p_current_menu->u8_highlighted_component_index = 0;
	}

	MENU_FSM_info.state = MENU_FSM_STATE_WAIT_FOR_MENU_EVENT;

	MENU_FSM_info.p_current_menu = p_menu->p_parent_menu;

	// Restore the default highlighted sub-menu (will always be the first one)
	if (MENU_FSM_info.p_current_menu->p_sub_menus)
	{
		MENU_FSM_info.p_current_menu->u8_highlighted_submenu_index = 0;
	}

	MENU_FSM_LOG_DBG("Current Menu: %s, Highlighted Sub-menu: %s\n", 
		p_menu->kpc_name, MENU_FSM_info.p_current_menu->p_sub_menus[p_menu->u8_highlighted_submenu_index].kpc_name);
}

static void MENU_FSM_step_into_sub_menu(void)
{
	MENU_FSM_info.p_current_menu = &MENU_FSM_info.p_current_menu->p_sub_menus[MENU_FSM_info.p_current_menu->u8_highlighted_submenu_index];

	switch (MENU_FSM_info.p_current_menu->id)
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

	MENU_FSM_info.state = MENU_FSM_STATE_WAIT_FOR_COMPONENT_EVENT;

	for (uint8_t i = 0; i < MENU_FSM_NUM_ON_SCREEN_TITLES; i++)
	{
		p_on_screen_titles_components[i].type = MENU_COMPONENT_TYPE_STORY;
		ppc_buffer_pointers[i] = p_on_screen_titles_components[i].component.story.pc_title;
	}

	MENU_FSM_LOG_DBG("Stories Menu Selected\n");
	
	status = DRIVE_API_fetch_fnames(MENU_FSM_NUM_ON_SCREEN_TITLES, MENU_FSM_ARC_PROJECT_FILTER, 0, ppc_buffer_pointers, &u8_num_found);

	if (status != ARCADIA_STATUS_OK)
	{
		MENU_FSM_LOG_WARN("Unable to retrieve file names (status: %u)\n", status);
	}
	
	MENU_FSM_LOG_DBG("Stories found: %u\n", u8_num_found);
}

/****************************************************************************************************
 *	C O M P O N E N T   N A V I G A T I O N
 ****************************************************************************************************/

static void MENU_FSM_go_to_component(MENU_FSM_direction_t direction)
{
	uint8_t u8_num_components 	= MENU_FSM_info.p_current_menu->u8_num_components;
	uint8_t * pu8_index 		= &MENU_FSM_info.p_current_menu->u8_highlighted_component_index;

	switch (MENU_FSM_info.p_current_menu->p_components[*pu8_index].type)
	{
		case MENU_COMPONENT_TYPE_STORY:
		{
			if (u8_num_components == 0)
			{
				break;
			}

			if (direction == MENU_FSM_DIRECTION_NEXT)
			{
				if (*pu8_index < u8_num_components - 1 && MENU_FSM_info.p_current_menu->p_components[(*pu8_index) + 1].component.story.pc_title[0] != '\0')
				{
					(*pu8_index)++;
				}
			}
			else if (direction == MENU_FSM_DIRECTION_PREV)
			{
				if (*pu8_index > 0)
				{
					(*pu8_index)--;
				}
			}

			MENU_FSM_LOG_DBG("Story[%u]: %s\n", *pu8_index, MENU_FSM_info.p_current_menu->p_components[*pu8_index].component.story.pc_title);
			break;
		}
	}
}
