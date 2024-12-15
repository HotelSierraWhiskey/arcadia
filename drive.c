#include "drive.h"
#include "common.h"
#include "shell.h"
#include "arcadia.h"
#include "chronos.h"
#include "nvmctrl.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define DRIVE_LOG_DBG(fmt, ...)   SHELL_printf("%-10s" fmt, "[DRIVE]", ##__VA_ARGS__)

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void DRIVE_handle_message(void);

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void DRIVE_init(void)
{
	// Initialize NVMCTRL module
	NVMCTRL_init();
}

void DRIVE_task(void * p_params)
{
	(void)p_params;

	while (1)
	{
		DRIVE_handle_message();
	}
}

static void DRIVE_handle_message(void)
{
	ARCADIA_msg_t msg;

	if (ARCADIA_receive(&msg))
	{
		switch (msg.id)
		{
			case ARCADIA_MSG_ID_NOOP:
				DRIVE_LOG_DBG("Received msg type %s from %s\r\n", 
					ARCADIA_get_msg_type(msg.id), ARCADIA_get_task_name(msg.from));
				break;
			default:
				DRIVE_LOG_DBG("Unexpected message: %u\r\n", msg.id);
		}
	}
}
