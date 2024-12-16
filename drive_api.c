#include "drive.h"
#include "drive_api.h"
#include "arcadia.h"
#include "drive_payload.h"
#include "shell.h"
#include "utils.h"
#include "nvmctrl.h"

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

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
	msg.semaphore = ARCADIA_semaphore_alloc(&msg.semaphore_buffer);

	msg.payload.drive_payload_write_nvm.u32_addr = ku32_addr;
	msg.payload.drive_payload_write_nvm.pc_buffer = pc_data;
	msg.payload.drive_payload_write_nvm.p_result_status = &status;

	ARCADIA_send(ARCADIA_TASK_ID_DRIVE, &msg);

	if (!ARCADIA_semaphore_take(msg.semaphore))
	{
		status = ARCADIA_STATUS_API_TIMEOUT;
	}

	ARCADIA_semaphore_free(msg.semaphore);

	return status;
}

ARCADIA_status_t DRIVE_API_erase_nvm(const uint32_t ku32_addr)
{
	ARCADIA_status_t 	status = ARCADIA_STATUS_FAILED;
	ARCADIA_msg_t 		msg;

	msg.id = ARCADIA_MSG_ID_DRIVE_ERASE_NVM;
	msg.from = ARCADIA_get_current_task_id();
	msg.semaphore = ARCADIA_semaphore_alloc(&msg.semaphore_buffer);

	msg.payload.drive_payload_erase_nvm.u32_addr = ku32_addr;
	msg.payload.drive_payload_erase_nvm.p_result_status = &status;

	ARCADIA_send(ARCADIA_TASK_ID_DRIVE, &msg);
	
	if (!ARCADIA_semaphore_take(msg.semaphore))
	{
		status = ARCADIA_STATUS_API_TIMEOUT;
	}

	ARCADIA_semaphore_free(msg.semaphore);

	return status;
}

uint8_t DRIVE_API_shell_erase_nvm(uint8_t argc, char ** argv)
{
	bool 		b_res = false;
	uint32_t 	u32_addr;
	
	if (argc == 1)
	{
		if (UTILS_string_to_u32(argv[0], &u32_addr))
		{
			if (u32_addr % NVMCTRL_PAGE_SIZE == 0)
			{
				DRIVE_API_erase_nvm(u32_addr);

				b_res = true;
			}
		}
	}
	if (!b_res)
	{
		SHELL_printf("Usage: drive nvm_erase <addr>\r\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

uint8_t DRIVE_API_shell_read_nvm(uint8_t argc, char ** argv)
{
	bool 		b_res = false;
	uint32_t 	u32_addr;
	char 		pc_buffer[NVMCTRL_PAGE_SIZE];

	if (argc == 1)
	{
		if (UTILS_string_to_u32(argv[0], &u32_addr))
		{
			if (ARCADIA_STATUS_OK == DRIVE_API_read_nvm((const uint32_t)u32_addr, pc_buffer))
			{
				SHELL_SEPARATOR();
				for (uint16_t i = 0; i < NVMCTRL_PAGE_SIZE; i++)
				{
					SHELL_printf("%02X ", pc_buffer[i]);

					if ((i + 1) % 16 == 0)
					{
						SHELL_printf("\r\n");
					}
				}
				SHELL_SEPARATOR();
			}
			b_res = true;
		}
	}
	
	if (!b_res)
	{
		SHELL_printf("Usage: drive nvm read <addr>\r\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

uint8_t DRIVE_API_shell_write_nvm(uint8_t argc, char ** argv)
{
	bool 		b_res = false;
	uint32_t 	u32_addr;
	uint32_t 	u32_num_bytes;
	uint32_t 	u32_data;
	char		pc_buffer[NVMCTRL_PAGE_SIZE];

	memset(pc_buffer, 0, NVMCTRL_PAGE_SIZE);

	if (argc >= 3)
	{
		if (UTILS_string_to_u32(argv[0], &u32_addr))
		{
			if (u32_addr % NVMCTRL_PAGE_SIZE == 0)
			{
				b_res = true;
			}
		}

		if (b_res && UTILS_string_to_u32(argv[1], &u32_num_bytes))
		{
			if (u32_num_bytes > NVMCTRL_PAGE_SIZE)
			{
				b_res = false;
			}
		}

		for (uint8_t i = 0; i < u32_num_bytes; i++)
		{
			if (b_res && UTILS_string_to_u32(argv[2 + i], &u32_data))
			{
				pc_buffer[i] = (uint8_t)u32_data;
			}
			else
			{
				SHELL_printf("Error: %s\r\n", argv[2 + i]);
				b_res = false;
				break;
			}
		}
	}
	
	if (b_res)
	{
		if (ARCADIA_STATUS_OK == DRIVE_API_write_nvm((const uint32_t) u32_addr, pc_buffer))
		{
			SHELL_printf("Wrote page to %0X\r\n", u32_addr);
		}
	}
	else
	{
		SHELL_printf("Usage: drive nvm write <addr> <num_bytes> <...>\r\n");
	}

	return SHELL_COMMAND_SUCCESS;
}
