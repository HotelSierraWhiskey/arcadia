#include "drive.h"
#include "drive_api.h"
#include "arcadia.h"
#include "drive_payload.h"
#include "shell.h"

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

// TODO: these should all take a sem and wait

ARCADIA_status_t DRIVE_API_read_nvm(const uint32_t ku32_addr, char * pc_data)
{
	ARCADIA_status_t 	status = ARCADIA_STATUS_FAILED;
	ARCADIA_msg_t 		msg;

	msg.id = ARCADIA_MSG_ID_DRIVE_READ_NVM;
	msg.from = ARCADIA_get_current_task_id();
	msg.semaphore = ARCADIA_semaphore_alloc(&msg.semaphore_buffer);

	msg.payload.drive_payload_read_nvm.u32_addr = ku32_addr;
	msg.payload.drive_payload_read_nvm.pc_buffer = pc_data;
	msg.payload.drive_payload_read_nvm.p_result_status = &status;

	ARCADIA_send(ARCADIA_TASK_ID_DRIVE, &msg);

	if (!ARCADIA_semaphore_take(msg.semaphore))
	{
		status = ARCADIA_STATUS_API_TIMEOUT;
	}

	ARCADIA_semaphore_free(msg.semaphore);

	return status;
}

ARCADIA_status_t DRIVE_API_write_nvm(const uint32_t ku32_addr, char * pc_data)
{
	ARCADIA_status_t 	status = ARCADIA_STATUS_FAILED;
	ARCADIA_msg_t 		msg;

	msg.id = ARCADIA_MSG_ID_DRIVE_WRITE_NVM;
	msg.from = ARCADIA_get_current_task_id();

	msg.payload.drive_payload_write_nvm.u32_addr = ku32_addr;
	msg.payload.drive_payload_write_nvm.pc_buffer = pc_data;

	if (ARCADIA_send(ARCADIA_TASK_ID_DRIVE, &msg))
	{
		status = ARCADIA_STATUS_OK;
	}
	else
	{
		status = ARCADIA_STATUS_FAILED;
	}

	return status;
}

ARCADIA_status_t DRIVE_API_erase_nvm(const uint32_t ku32_addr)
{
	ARCADIA_status_t 	status = ARCADIA_STATUS_FAILED;
	ARCADIA_msg_t 		msg;

	msg.id = ARCADIA_MSG_ID_DRIVE_ERASE_NVM;
	msg.from = ARCADIA_get_current_task_id();

	msg.payload.drive_payload_erase_nvm.u32_addr = ku32_addr;

	if (ARCADIA_send(ARCADIA_TASK_ID_DRIVE, &msg))
	{
		status = ARCADIA_STATUS_OK;
	}
	else
	{
		status = ARCADIA_STATUS_FAILED;
	}

	return status;
}
