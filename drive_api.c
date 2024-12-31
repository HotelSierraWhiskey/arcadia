#include "drive.h"
#include "drive_api.h"
#include "arcadia.h"
#include "drive_payload.h"
#include "shell.h"
#include "utils.h"
#include "nvmctrl.h"
#include "fsif.h"
#include "chrono.h"

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
 *	S H E L L   F U N C T I O N S
 ****************************************************************************************************/

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

/****************************************************************************************************
 *	Shell utility
 *
 * 	Runs the interface wrapper for FatFs f_mkfs
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t DRIVE_API_shell_mkfs(uint8_t argc, char ** argv)
{
	uint32_t 	u32_start_ticks = CHRONO_get_ticks();
	uint32_t 	u32_ticks_elapsed;
	FRESULT 	f_result;

	if (argc == 0)
	{
		f_result = FSIF_f_mkfs();

		if (FR_OK == f_result)
		{
			u32_ticks_elapsed = CHRONO_ticks_since(u32_start_ticks);
			SHELL_printf("Formatted volume\r\n");
			SHELL_printf("Time elapsed: %u.%u seconds\r\n",
				u32_ticks_elapsed / 1000, u32_ticks_elapsed % 1000);
		}
		else
		{
			SHELL_printf("FSIF_f_mkfs returned status %u\r\n", f_result);
		}
	}
	else
	{
		SHELL_printf("Usage: drive fs format\r\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

/****************************************************************************************************
 *	Shell utility
 *
 * 	Runs the interface wrapper for FatFs f_mount
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t DRIVE_API_shell_mount(uint8_t argc, char ** argv)
{
	FRESULT f_result;

	if (argc == 0)
	{
		f_result = FSIF_f_mount();

		if (FR_OK == f_result)
		{
			SHELL_printf("File system mounted\r\n");
		}
	}
	else
	{
		SHELL_printf("Usage: drive fs mount\r\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

/****************************************************************************************************
 *	Shell utility
 *
 * 	Runs the interface wrapper for FatFs f_open
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t DRIVE_API_shell_open(uint8_t argc, char ** argv)
{
	char buf[32];
	memset(buf, 0, 32);
	uint32_t bw = 0;
	uint32_t br = 0;
	FRESULT res;

	if (argc == 0)
	{
		res = FSIF_f_open("beep.txt", buf, &bw, &br);

		SHELL_printf("res: %u, bw: %u, br: %u\r\n", res, bw, br);

		if (res == FR_OK)
		{
			SHELL_printf("buffer: %s\r\n", buf);
		}

	}
	else
	{
		SHELL_printf("Usage: drive fs open\r\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

/****************************************************************************************************
 *	Shell utility
 *
 * 	Lists file system contents
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t DRIVE_API_shell_ls(uint8_t argc, char ** argv)
{
	FRESULT		f_result;
	DIR			dir_obj;
	FILINFO 	f_info;
	uint8_t		u8_num_files = 0;
	FATFS *		fs;
	DWORD 		free_clusters, free_sectors, total_sectors;

	if (argc == 0)
	{
		f_result = f_findfirst(&dir_obj, &f_info, "", "*.*");

		SHELL_SEPARATOR();
		SHELL_printf("%-20s %s\r\n", "file", "size");
		SHELL_SEPARATOR();

		while (f_result == FR_OK && f_info.fname[0])
		{
			u8_num_files++;
			SHELL_printf("%-20s %u\r\n", f_info.fname, f_info.fsize);
			f_result = f_findnext(&dir_obj, &f_info);
		}

		f_closedir(&dir_obj);

		SHELL_printf("\r\nTotal: %u\r\n", u8_num_files);

		f_result = f_getfree("", &free_clusters, &fs);

		SHELL_SEPARATOR();
		SHELL_printf("Number of FAT entries: %u\r\n", fs->n_fatent);
		SHELL_printf("Cluster Size: %u sectors\r\n", fs->csize);
		SHELL_printf("Free Clusters: %u\r\n", free_clusters);

		total_sectors = (fs->n_fatent - 2) * fs->csize;
    	free_sectors = free_clusters * fs->csize;

		/* Print the free space (assuming 512 bytes/sector) */
		SHELL_printf("Total drive space: %10lu KiB\r\n", total_sectors / 2);
		SHELL_printf("Available space: %10lu KiB\r\n", free_sectors / 2);
		SHELL_SEPARATOR();
	}
	else
	{
		SHELL_printf("Usage: ls\r\n");
	}

	return SHELL_COMMAND_SUCCESS;
}
