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

/****************************************************************************************************
 *	Blocking API function to read a row of data
 *
 * 	@param[in]	k_row_id The ID of the desired NVM row from which to read
 * 	@param[out]	pc_data A buffer to load with the retrieved data
 * 
 *	@return 
 * 	`ARCADIA_STATUS_OK` if successful, 
 *	`ARCADIA_STATUS_API_TIMEOUT` if the DRIVE task was busy
 *	`ARCADIA_STATUS_FAILED` if something went wrong
 ****************************************************************************************************/
ARCADIA_status_t DRIVE_API_read_nvm(const NVMCTRL_app_nvm_row_id_t k_row_id, char * pc_data)
{
	ARCADIA_status_t status = ARCADIA_STATUS_FAILED;

	DRIVE_PAYLOAD_read_nvm_t payload =
	{
		.u32_addr = NVMCTRL_get_addr_from_row_id(k_row_id),
		.pc_buffer = pc_data,
		.p_result_status = &status,
	};

	ARCADIA_msg_t msg =
	{
		.id = ARCADIA_MSG_ID_DRIVE_READ_NVM,
		.from = ARCADIA_get_current_task_id(),
		.payload.drive_payload_read_nvm = payload
	};

	msg.semaphore = ARCADIA_semaphore_alloc(&msg.semaphore_buffer);

	ARCADIA_send(ARCADIA_TASK_ID_DRIVE, &msg);

	if (!ARCADIA_semaphore_take(msg.semaphore))
	{
		status = ARCADIA_STATUS_API_TIMEOUT;
	}

	ARCADIA_semaphore_free(msg.semaphore);

	return status;
}

/****************************************************************************************************
 *	Blocking API function to write a row of data
 *
 * 	@param[in]	k_row_id The ID of the desired NVM row at which to write
 * 	@param[in]	kpc_data The row of data to write
 * 
 *	@return 
 * 	`ARCADIA_STATUS_OK` if successful, 
 *	`ARCADIA_STATUS_API_TIMEOUT` if the DRIVE task was busy
 *	`ARCADIA_STATUS_FAILED` if something went wrong
 ****************************************************************************************************/
ARCADIA_status_t DRIVE_API_write_nvm(const NVMCTRL_app_nvm_row_id_t k_row_id, const char * kpc_data)
{
	ARCADIA_status_t status = ARCADIA_STATUS_FAILED;

	DRIVE_PAYLOAD_write_nvm_t payload =
	{
		.u32_addr = NVMCTRL_get_addr_from_row_id(k_row_id),
		.kpc_buffer = kpc_data,
		.p_result_status = &status
	};

	ARCADIA_msg_t msg =
	{
		.id = ARCADIA_MSG_ID_DRIVE_WRITE_NVM,
		.from = ARCADIA_get_current_task_id(),
		.payload.drive_payload_write_nvm = payload
	};

	msg.semaphore = ARCADIA_semaphore_alloc(&msg.semaphore_buffer);

	ARCADIA_send(ARCADIA_TASK_ID_DRIVE, &msg);

	if (!ARCADIA_semaphore_take(msg.semaphore))
	{
		status = ARCADIA_STATUS_API_TIMEOUT;
	}

	ARCADIA_semaphore_free(msg.semaphore);

	return status;
}

/****************************************************************************************************
 *	Blocking API function to erase a row of data
 *
 * 	@param[in] k_row_id The ID of the desired row to erase
 * 
 *	@return 
 * 	`ARCADIA_STATUS_OK` if successful, 
 *	`ARCADIA_STATUS_API_TIMEOUT` if the DRIVE task was busy
 *	`ARCADIA_STATUS_FAILED` if something went wrong
 ****************************************************************************************************/
ARCADIA_status_t DRIVE_API_erase_nvm(const NVMCTRL_app_nvm_row_id_t k_row_id)
{
	ARCADIA_status_t 	status = ARCADIA_STATUS_FAILED;

	DRIVE_PAYLOAD_erase_nvm_t payload =
	{
		.u32_addr = NVMCTRL_get_addr_from_row_id(k_row_id),
		.p_result_status = &status
	};

	ARCADIA_msg_t msg =
	{
		.id = ARCADIA_MSG_ID_DRIVE_ERASE_NVM,
		.from = ARCADIA_get_current_task_id(),
		.payload.drive_payload_erase_nvm = payload
	};

	msg.semaphore = ARCADIA_semaphore_alloc(&msg.semaphore_buffer);

	ARCADIA_send(ARCADIA_TASK_ID_DRIVE, &msg);
	
	if (!ARCADIA_semaphore_take(msg.semaphore))
	{
		status = ARCADIA_STATUS_API_TIMEOUT;
	}

	ARCADIA_semaphore_free(msg.semaphore);

	return status;
}

/****************************************************************************************************
 *	Shell utility
 *
 * 	Erases a row of data at the address of the provided row ID
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t DRIVE_API_shell_erase_nvm(uint8_t argc, char ** argv)
{
	bool 		b_res = false;
	uint32_t 	u32_row;
	
	if (argc == 1)
	{
		if (UTILS_string_to_u32(argv[0], &u32_row))
		{
			if (u32_row < NVMCTRL_APP_NVM_ROW_NUM_ROWS)
			{
				DRIVE_API_erase_nvm((const NVMCTRL_app_nvm_row_id_t)u32_row);

				b_res = true;
			}
		}
	}
	if (!b_res)
	{
		SHELL_printf("Usage: drive nvm_erase <row_id>\r\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

/****************************************************************************************************
 *	Shell utility
 *
 * 	Reads and displays a row of data from the address of the provided row ID
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t DRIVE_API_shell_read_nvm(uint8_t argc, char ** argv)
{
	bool 		b_res = false;
	uint32_t 	u32_row;
	char 		pc_buffer[NVMCTRL_ROW_SIZE];

	if (argc == 1)
	{
		if (UTILS_string_to_u32(argv[0], &u32_row))
		{
			if (u32_row < NVMCTRL_APP_NVM_ROW_NUM_ROWS)
			{
				if (ARCADIA_STATUS_OK == DRIVE_API_read_nvm((const NVMCTRL_app_nvm_row_id_t)u32_row, pc_buffer))
				{
					SHELL_SEPARATOR();
					for (uint8_t u8_page = 0; u8_page < NVMCTRL_ROW_SIZE / NVMCTRL_PAGE_SIZE; u8_page++)
					{
						for (uint16_t i = 0; i < NVMCTRL_PAGE_SIZE; i++)
						{
							SHELL_printf("%02X ", pc_buffer[i + (NVMCTRL_PAGE_SIZE * u8_page)]);

							if ((i + 1) % 16 == 0)
							{
								SHELL_printf("\r\n");
							}
						}
						if (u8_page < (NVMCTRL_ROW_SIZE / NVMCTRL_PAGE_SIZE - 1))
						{
							SHELL_printf("\r\n");
						}
					}
					SHELL_SEPARATOR();
				}
			}
			b_res = true;
		}
	}
	
	if (!b_res)
	{
		SHELL_printf("Usage: drive nvm read <row_id>\r\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

/****************************************************************************************************
 *	Shell utility
 *
 * 	Writes a row of data to the address of the provided row ID
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t DRIVE_API_shell_write_nvm(uint8_t argc, char ** argv)
{
	bool 				b_res = false;
	uint32_t 			u32_row;
	uint32_t 			u32_num_bytes;
	uint32_t 			u32_data;
	char				pc_buffer[NVMCTRL_ROW_SIZE];
	ARCADIA_status_t	status;

	memset(pc_buffer, 0, NVMCTRL_ROW_SIZE);

	if (argc >= 3)
	{
		if (UTILS_string_to_u32(argv[0], &u32_row))
		{
			if (u32_row < NVMCTRL_APP_NVM_ROW_NUM_ROWS)
			{
				b_res = true;
			}
		}

		if (b_res && UTILS_string_to_u32(argv[1], &u32_num_bytes))
		{
			if(u32_num_bytes != argc - 2 || u32_num_bytes > NVMCTRL_ROW_SIZE)
			{
				b_res = false;
			}
		}

		if (b_res)
		{
			for (uint8_t i = 0; i < u32_num_bytes; i++)
			{
				if (UTILS_string_to_u32(argv[2 + i], &u32_data))
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
	}
	
	if (b_res)
	{
		status = DRIVE_API_write_nvm((const NVMCTRL_app_nvm_row_id_t)u32_row, pc_buffer);

		if (status != ARCADIA_STATUS_OK)
		{
			SHELL_printf("Failed to write to nvm row #%u (status: %u)\r\n", u32_row, status);
		}
	}
	else
	{
		SHELL_printf("Usage: drive nvm write <row_id> <num_bytes> <...>\r\n");
	}

	return SHELL_COMMAND_SUCCESS;
}
