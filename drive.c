#include "drive.h"
#include "common.h"
#include "shell.h"
#include "arcadia.h"
#include "chrono.h"
#include "nvmctrl.h"
#include "utils.h"
#include "drive_payload.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define DRIVE_LOG_DBG(fmt, ...)   SHELL_printf("%-10s" fmt, "[DRIVE]", ##__VA_ARGS__)

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void 	DRIVE_handle_message		(void);
static void 	DRIVE_handle_msg_noop		(ARCADIA_msg_t * p_msg);
static void 	DRIVE_handle_msg_read_nvm	(ARCADIA_msg_t * p_msg);
static void 	DRIVE_handle_msg_write_nvm	(ARCADIA_msg_t * p_msg);
static void 	DRIVE_handle_msg_erase_nvm	(ARCADIA_msg_t * p_msg);

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
				DRIVE_handle_msg_noop(&msg);
				break;

			case ARCADIA_MSG_ID_DRIVE_READ_NVM:
				DRIVE_handle_msg_read_nvm(&msg);
				break;

			case ARCADIA_MSG_ID_DRIVE_WRITE_NVM:
				DRIVE_handle_msg_write_nvm(&msg);
				break;

			case ARCADIA_MSG_ID_DRIVE_ERASE_NVM:
				DRIVE_handle_msg_erase_nvm(&msg);
				break;
			
			default:
				DRIVE_LOG_DBG("Unexpected message: %u\r\n", msg.id);
		}
	}
}

static void DRIVE_handle_msg_noop(ARCADIA_msg_t * p_msg)
{
	DRIVE_LOG_DBG("Received msg type %s from %s\r\n", 
		ARCADIA_get_msg_type(p_msg->id), ARCADIA_get_task_name(p_msg->from));
}

static void	DRIVE_handle_msg_read_nvm(ARCADIA_msg_t * p_msg)
{
	uint32_t 	u32_addr = p_msg->payload.drive_payload_read_nvm.u32_addr;
	uint8_t * 	pc_buffer = (uint8_t *)p_msg->payload.drive_payload_read_nvm.pc_buffer;

	DRIVE_LOG_DBG("Received msg type %s from %s\r\n", 
		ARCADIA_get_msg_type(p_msg->id), ARCADIA_get_task_name(p_msg->from));

	for (uint16_t i = 0; i < NVMCTRL_PAGE_SIZE; i++)
	{
		pc_buffer[i] = ((uint8_t *)NVMCTRL_MEMORY)[u32_addr + i];
	}
}

static void DRIVE_handle_msg_write_nvm(ARCADIA_msg_t * p_msg)
{
	uint32_t 	u32_addr = p_msg->payload.drive_payload_write_nvm.u32_addr;
	uint8_t * 	pc_buffer = (uint8_t *)p_msg->payload.drive_payload_write_nvm.pc_buffer;

	DRIVE_LOG_DBG("Received msg type %s from %s\r\n", 
		ARCADIA_get_msg_type(p_msg->id), ARCADIA_get_task_name(p_msg->from));

	NVMCTRL_write_page(u32_addr, pc_buffer);
}

static void DRIVE_handle_msg_erase_nvm(ARCADIA_msg_t * p_msg)
{
	uint32_t 	u32_addr = p_msg->payload.drive_payload_erase_nvm.u32_addr;

	DRIVE_LOG_DBG("Received msg type %s from %s\r\n", 
		ARCADIA_get_msg_type(p_msg->id), ARCADIA_get_task_name(p_msg->from));

	NVMCTRL_erase_row(u32_addr);
}
