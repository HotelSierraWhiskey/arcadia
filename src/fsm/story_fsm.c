#include "story_fsm.h"
#include "app_fsm.h"
#include "utils.h"
#include "mempool.h"
#include "drive_api.h"
#include "arcproject.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define STORY_FSM_LOG_DBG(fmt, ...)   		SHELL_printf("\r%-12s" fmt, "[STORY_FSM]", ##__VA_ARGS__)
#define STORY_FSM_LOG_WARN(fmt, ...)   		SHELL_PRINT_WARNING("\r%-12s" fmt, "[STORY_FSM]", ##__VA_ARGS__)

#define STORY_FSM_CHUNK_BUFFER_SIZE			(512U)

typedef struct _STORY_FSM_info_
{
	MEMPOOL_buffer_t 	p_buffer;
	file_t *			p_file;
	char 				pc_arcproject[COMMON_MAX_FNAME_SIZE];
} STORY_FSM_info_t;

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void 	STORY_FSM_advance	(void);

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

STORY_FSM_info_t STORY_FSM_info;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void STORY_FSM_init(void)
{
	STORY_FSM_info.p_buffer = MEMPOOL_alloc(MEMPOOL_BUFFER_SIZE_ID_512);
}

void STORY_FSM_handle_event(FSM_EVENT_t event)
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

void STORY_FSM_load_arcproject(const char * kpc_arcproject)
{
	ASSERT(kpc_arcproject);

	ARCADIA_status_t 	status;
	file_t * 			p_file = NULL;

	strncpy(STORY_FSM_info.pc_arcproject, kpc_arcproject, COMMON_MAX_FNAME_SIZE);

	status = DRIVE_API_chdir(STORY_FSM_info.pc_arcproject);

	if (status != ARCADIA_STATUS_OK)
	{
		STORY_FSM_LOG_WARN("Unable to load %s (status: %u)\n", STORY_FSM_info.pc_arcproject, status);
		return;
	}

	status = DRIVE_API_open_file(p_file, ARCPROJECT_BOOKMARK_FILENAME, "r");



	// ARCPROJECT_bookmark_get_node();

	STORY_FSM_LOG_DBG("Loaded arcproject: %s\n", STORY_FSM_info.pc_arcproject);
}

static void STORY_FSM_advance(void)
{

}