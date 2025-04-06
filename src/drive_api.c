#include "drive.h"
#include "drive_api.h"
#include "arcadia.h"
#include "drive_payload.h"
#include "shell.h"
#include "utils.h"
#include "nvmctrl.h"
#include "fsif.h"
#include "chrono.h"
#include "mempool.h"

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
	ARCADIA_status_t status = ARCADIA_STATUS_FAILED;

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

ARCADIA_status_t DRIVE_API_open_file(file_handle_t * p_file_handle, const char *kpc_fname, const char * kpc_open_mode)
{
	ARCADIA_status_t status = ARCADIA_STATUS_FAILED;

	DRIVE_PAYLOAD_open_file_t payload =
	{
		.p_file_handle = p_file_handle,
		.kpc_fname = kpc_fname,
		.kpc_open_mode = kpc_open_mode,
		.p_result_status = &status
	};

	ARCADIA_msg_t msg =
	{
		.id = ARCADIA_MSG_ID_DRIVE_OPEN_FILE,
		.from = ARCADIA_get_current_task_id(),
		.payload.drive_payload_open_file = payload
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

ARCADIA_status_t DRIVE_API_close_file(file_handle_t file_handle)
{
	ARCADIA_status_t status = ARCADIA_STATUS_FAILED;

	DRIVE_PAYLOAD_close_file_t payload =
	{
		.file_handle = file_handle,
		.p_result_status = &status
	};

	ARCADIA_msg_t msg =
	{
		.id = ARCADIA_MSG_ID_DRIVE_CLOSE_FILE,
		.from = ARCADIA_get_current_task_id(),
		.payload.drive_payload_close_file = payload
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

ARCADIA_status_t DRIVE_API_fetch_fnames(uint8_t u8_num_fnames, const char * kpc_filter, uint8_t u8_start_index, char ** ppc_buffer, uint8_t * pu8_num_found)
{
	ASSERT(ppc_buffer);
	ASSERT(pu8_num_found);

	ARCADIA_status_t status = ARCADIA_STATUS_FAILED;

	DRIVE_PAYLOAD_fetch_fnames_t payload =
	{
		.u8_num_fnames 		= u8_num_fnames,
		.kpc_filter			= kpc_filter,
		.u8_start_index 	= u8_start_index,
		.ppc_buffer 		= ppc_buffer,
		.pu8_num_found 		= pu8_num_found,
		.p_result_status 	= &status
	};

	ARCADIA_msg_t msg =
	{
		.id = ARCADIA_MSG_ID_DRIVE_FETCH_FNAMES,
		.from = ARCADIA_get_current_task_id(),
		.payload.drive_payload_fetch_fnames = payload
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

ARCADIA_status_t DRIVE_API_chdir(const char * kpc_dirname)
{
	ASSERT(kpc_dirname);

	ARCADIA_status_t status = ARCADIA_STATUS_FAILED;

	DRIVE_PAYLOAD_chdir_t payload =
	{
		.kpc_dirname = kpc_dirname,
		.p_result_status = &status
	};

	ARCADIA_msg_t msg =
	{
		.id = ARCADIA_MSG_ID_DRIVE_CHDIR,
		.from = ARCADIA_get_current_task_id(),
		.payload.drive_payload_chdir = payload
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

ARCADIA_status_t DRIVE_API_write(file_handle_t file_handle, const char * kpc_data)
{
	ARCADIA_status_t status = ARCADIA_STATUS_FAILED;

	DRIVE_PAYLOAD_write_t payload =
	{
		.file_handle = file_handle,
		.kpc_data = kpc_data,
		.p_result_status = &status
	};

	ARCADIA_msg_t msg =
	{
		.id = ARCADIA_MSG_ID_DRIVE_WRITE,
		.from = ARCADIA_get_current_task_id(),
		.payload.drive_payload_write = payload
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

ARCADIA_status_t DRIVE_API_read(file_handle_t file_handle, char * pc_data, uint32_t u32_bytes_to_read)
{
	ARCADIA_status_t status = ARCADIA_STATUS_FAILED;

	DRIVE_PAYLOAD_read_t payload =
	{
		.file_handle = file_handle,
		.pc_data = pc_data,
		.u32_bytes_to_read = u32_bytes_to_read,
		.p_result_status = &status
	};

	ARCADIA_msg_t msg =
	{
		.id = ARCADIA_MSG_ID_DRIVE_READ,
		.from = ARCADIA_get_current_task_id(),
		.payload.drive_payload_read = payload
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
		SHELL_printf("Usage: drive nvm_erase <row_id>\n");
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
	bool 				b_res = false;
	uint32_t 			u32_row;
	MEMPOOL_buffer_t	buffer = MEMPOOL_alloc(MEMPOOL_BUFFER_SIZE_ID_256);

	if (argc == 1)
	{
		if (UTILS_string_to_u32(argv[0], &u32_row))
		{
			if (u32_row < NVMCTRL_APP_NVM_ROW_NUM_ROWS)
			{
				if (ARCADIA_STATUS_OK == DRIVE_API_read_nvm((const NVMCTRL_app_nvm_row_id_t)u32_row, buffer))
				{
					SHELL_SEPARATOR();
					for (uint8_t u8_page = 0; u8_page < NVMCTRL_ROW_SIZE / NVMCTRL_PAGE_SIZE; u8_page++)
					{
						for (uint16_t i = 0; i < NVMCTRL_PAGE_SIZE; i++)
						{
							SHELL_printf("%02X ", ((char *)(buffer))[i + (NVMCTRL_PAGE_SIZE * u8_page)]);

							if ((i + 1) % 16 == 0)
							{
								SHELL_printf("\n");
							}
						}
						if (u8_page < (NVMCTRL_ROW_SIZE / NVMCTRL_PAGE_SIZE - 1))
						{
							SHELL_printf("\n");
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
		SHELL_printf("Usage: drive nvm read <row_id>\n");
	}

	MEMPOOL_free(buffer);

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
	ARCADIA_status_t	status;
	MEMPOOL_buffer_t	pc_buffer = MEMPOOL_alloc(MEMPOOL_BUFFER_SIZE_ID_256);

	ASSERT(pc_buffer);

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
					((char *)(pc_buffer))[i] = (uint8_t)u32_data;
				}
				else
				{
					SHELL_printf("Error: %s\n", argv[2 + i]);
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
			SHELL_printf("Failed to write to nvm row #%u (status: %u)\n", u32_row, status);
		}
	}
	else
	{
		SHELL_printf("Usage: drive nvm write <row_id> <num_bytes> <...>\n");
	}

	MEMPOOL_free(pc_buffer);

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
uint8_t DRIVE_API_shell_cat(uint8_t argc, char ** argv)
{
	FRESULT 			f_result;
	FIL					file;
	UINT				u32_bytes_read = 0;
	UINT				u32_bytes_this_read = 0;
	UINT				u32_bytes_to_read;
	UINT				u32_file_size;
	const uint16_t		ku16_block = 512;
	MEMPOOL_buffer_t	file_buffer = MEMPOOL_alloc(MEMPOOL_BUFFER_SIZE_ID_512);

	ASSERT(file_buffer);

	memset(file_buffer, 0, ku16_block);

	if (argc == 1)
	{

		f_result = f_open(&file, argv[0], FA_READ);

		if (FR_OK == f_result)
		{
			u32_file_size = f_size(&file);

			if (u32_file_size > 0)
			{
				if (u32_file_size < ku16_block)
				{
					f_result = f_read(&file, file_buffer, u32_file_size, &u32_bytes_read);
					SHELL_printf("%s\n", (char *)file_buffer);
				}
				else
				{	
					while (u32_bytes_read < u32_file_size && f_result == FR_OK)
					{
						u32_bytes_this_read = 0;
						u32_bytes_to_read = (u32_file_size - u32_bytes_read > ku16_block)
											? ku16_block
											: (u32_file_size - u32_bytes_read);

						f_result = f_read(&file, file_buffer, u32_bytes_to_read, &u32_bytes_read);

						if (f_result == FR_OK)
						{
							// f_read will return OK at EOF, but won't read anything. So break here.
							if (u32_bytes_read == 0)
							{
								break;
							}

							SHELL_printf("%s\n", (char *)file_buffer);
							memset(file_buffer, 0, ku16_block);
							u32_bytes_read += u32_bytes_this_read;
						}
						// Break if anything else is the case
						else
						{
							break;
						}
					}
				}
			}
			
			f_close(&file);
		}
		else
		{
			SHELL_printf("Couldn't open file: %s (status: %u)\n", argv[0], f_result);
		}
	}
	else
	{
		SHELL_printf("Usage: drive fs cat <fname>\n");
	}

	MEMPOOL_free(file_buffer);

	return SHELL_COMMAND_SUCCESS;
}

uint8_t DRIVE_API_shell_pwd(uint8_t argc, char ** argv)
{
	MEMPOOL_buffer_t 	buffer = MEMPOOL_alloc(MEMPOOL_BUFFER_SIZE_ID_256);
	FRESULT				f_result;

	if (argc == 0)
	{
		f_result = f_getcwd((TCHAR *)buffer, MEMPOOL_BUFFER_SIZE_256);

		if (FR_OK == f_result)
		{
			SHELL_printf("%s\n", (char *)buffer);
		}
		else
		{
			SHELL_printf("Couldn't get current working directory (status : %u)\n", f_result);
		}
	}
	else
	{
		SHELL_printf("Usage: pwd\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

uint8_t DRIVE_API_shell_cd(uint8_t argc, char ** argv)
{
	FRESULT f_result;

	if (argc == 1)
	{
		f_result = f_chdir((TCHAR *)argv[0]);

		if (FR_OK == f_result)
		{
			SHELL_printf("Ok\n");
		}
		else
		{
			SHELL_printf("Failed to change directory to %s (status : %u)\n", argv[0], f_result);
		}
	}
	else
	{
		SHELL_printf("Usage: cd <dir>\n");
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
			SHELL_printf("Formatted volume\n");
			SHELL_printf("Time elapsed: %u.%u seconds\n",
				u32_ticks_elapsed / 1000, u32_ticks_elapsed % 1000);
		}
		else
		{
			SHELL_printf("FSIF_f_mkfs returned status %u\n", f_result);
		}
	}
	else
	{
		SHELL_printf("Usage: drive fs format\n");
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
			SHELL_printf("File system mounted\n");
		}
		else
		{
			SHELL_printf("Failed to mount file system (status: %u)\n", f_result);
		}
	}
	else
	{
		SHELL_printf("Usage: drive fs mount\n");
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
uint8_t DRIVE_API_shell_touch(uint8_t argc, char ** argv)
{
	file_t		file;
	FRESULT 	f_result;

	if (argc == 1)
	{
    	f_result = f_open(&file, argv[0], FA_READ | FA_OPEN_ALWAYS);

		if (FR_OK == f_result)
		{
			SHELL_printf("Created file: %s\n", argv[0]);
			f_close(&file);
		}
		else
		{
			SHELL_printf("Failed to create file, (status: %u)\n", f_result);
		}
	}
	else
	{
		SHELL_printf("Usage: touch\n");
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
	file_handle_t file_handle = FSIF_INVALID_FILE;

	if (argc == 1)
	{
		DRIVE_API_open_file(&file_handle, argv[0], "r");
	}
	else
	{
		SHELL_printf("Usage: drive fs open <fname>\n");
	}
	
	return SHELL_COMMAND_SUCCESS;
}

uint8_t DRIVE_API_shell_close(uint8_t argc, char ** argv)
{
	uint32_t file_handle;

	if (argc == 1)
	{
		if (UTILS_string_to_u32(argv[0], &file_handle))
		{
			DRIVE_API_close_file(file_handle);
		}
	}
	else
	{
		SHELL_printf("Usage: drive fs open <fname>\n");
	}
	
	return SHELL_COMMAND_SUCCESS;
}

uint8_t DRIVE_API_shell_read(uint8_t argc, char ** argv)
{
	return SHELL_COMMAND_SUCCESS;
}

uint8_t DRIVE_API_shell_rm(uint8_t argc, char ** argv)
{
	FRESULT 	f_result;

	if (argc == 1)
	{
		f_result = f_unlink(argv[0]);

		if (FR_OK == f_result)
		{
			SHELL_printf("Deleted file: %s\n", argv[0]);
		}
		else
		{
			SHELL_printf("Failed to delete file (status: %u)\n", f_result);
		}
	}
	else
	{
		SHELL_printf("Usage: drive fs open <fname>\n");
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
	FRESULT				f_result;
	DIR					dir_obj;
	FILINFO 			f_info;
	uint8_t				u8_num_files = 0;
	uint8_t				u8_num_dirs = 0;
	MEMPOOL_buffer_t 	buffer = MEMPOOL_alloc(MEMPOOL_BUFFER_SIZE_ID_256);

	if (argc == 0)
	{
		f_result = f_getcwd((TCHAR *)buffer, MEMPOOL_BUFFER_SIZE_512);

		if (FR_OK == f_result)
		{
			f_result = f_opendir(&dir_obj, buffer);

			if (FR_OK == f_result)
			{

				SHELL_SEPARATOR();
				SHELL_printf("%-24s %-24s %-24s\n", "name", "size", "type");
				SHELL_SEPARATOR();

				while (1)
				{
					f_result = f_readdir(&dir_obj, &f_info);

					if (f_result != FR_OK || f_info.fname[0] == 0)
					{
						break;
					}
					else if (f_info.fattrib & AM_DIR)
					{
						SHELL_printf("%-24s %-24s %s\n", f_info.fname, "--", "dir");
						u8_num_dirs++;
					}
					else
					{
						SHELL_printf("%-24s %-24u %s\n", f_info.fname, f_info.fsize, "file");
						u8_num_files++;
					}
				}
			}

			SHELL_SEPARATOR();
		}
	}
	else
	{
		SHELL_printf("Usage: ls\n");
	}

	MEMPOOL_free(buffer);

	return SHELL_COMMAND_SUCCESS;
}

uint8_t DRIVE_API_shell_unmount(uint8_t argc, char ** argv)
{
	FRESULT 	f_result;

	if (argc == 0)
	{
		f_result = f_unmount("");

		if (FR_OK == f_result)
		{
			SHELL_printf("Filesystem unmounted\n");
		}
		else
		{
			SHELL_printf("Failed to unmount file system (status: %u)\n", f_result);
		}
	}
	else
	{
		SHELL_printf("Usage: drive fs unmount\n");
	}
	
	return SHELL_COMMAND_SUCCESS;
}

uint8_t DRIVE_API_shell_fs_info(uint8_t argc, char ** argv)
{
	FRESULT 	f_result;
	FATFS *		fs;
	DWORD 		free_clusters, free_sectors, total_sectors;

	if (argc == 0)
	{
		f_result = f_getfree("", &free_clusters, &fs);

		if (FR_OK == f_result)
		{
			SHELL_SEPARATOR();
			SHELL_printf("Number of FAT entries: %u\n", fs->n_fatent);
			SHELL_printf("Cluster Size: %u sectors\n", fs->csize);
			SHELL_printf("Free Clusters: %u\n", free_clusters);

			total_sectors = (fs->n_fatent - 2) * fs->csize;
			free_sectors = free_clusters * fs->csize;

			// 1Kib / 2 = sector size, so divide total and free by two
			SHELL_printf("Total drive space: %10lu KiB\n", total_sectors / 2);
			SHELL_printf("Available space: %10lu KiB\n", free_sectors / 2);
			SHELL_SEPARATOR();
		}
		else
		{
			SHELL_printf("Failed to get file system info (status: %u)\n", f_result);
		}
	}
	else
	{
		SHELL_printf("Usage: drive fs info\n");
	}
	
	return SHELL_COMMAND_SUCCESS;
}
