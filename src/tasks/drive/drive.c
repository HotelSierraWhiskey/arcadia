#include "drive.h"
#include "common.h"
#include "shell.h"
#include "arcadia.h"
#include "chrono.h"
#include "nvmctrl.h"
#include "utils.h"
#include "drive_payload.h"
#include "spi.h"
#include "fsif.h"
#include "mempool.h"
#include "exti.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define DRIVE_LOG_DBG(fmt, ...)   		SHELL_printf("\r%-12s" fmt, "[DRIVE]", ##__VA_ARGS__)
#define DRIVE_LOG_WARN(fmt, ...)   		SHELL_PRINT_WARNING("\r%-12s" fmt, "[DRIVE]", ##__VA_ARGS__)

#define DRIVE_MAX_OPEN_FILES			(3U)

typedef struct _DRIVE_file_slot
{
	file_t 				file;
	bool 				b_in_use;
	DRIVE_file_handle_t file_handle;
} DRIVE_file_slot_t;
typedef struct _DRIVE_info
{
	DRIVE_file_slot_t file_pool[DRIVE_MAX_OPEN_FILES];
} DRIVE_info_t;

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void 		DRIVE_handle_message					(void);
static void 		DRIVE_handle_msg_read_nvm				(ARCADIA_msg_t * p_msg);
static void 		DRIVE_handle_msg_write_nvm				(ARCADIA_msg_t * p_msg);
static void 		DRIVE_handle_msg_erase_nvm				(ARCADIA_msg_t * p_msg);

static void 		DRIVE_handle_msg_open_file				(ARCADIA_msg_t * p_msg);
static void 		DRIVE_handle_msg_close_file				(ARCADIA_msg_t * p_msg);
static void 		DRIVE_handle_msg_chdir					(ARCADIA_msg_t * p_msg);
static void			DRIVE_handle_msg_fetch_fnames			(ARCADIA_msg_t * p_msg);
static void			DRIVE_handle_msg_write					(ARCADIA_msg_t * p_msg);
static void			DRIVE_handle_msg_read					(ARCADIA_msg_t * p_msg);
static void			DRIVE_handle_msg_seek					(ARCADIA_msg_t * p_msg);

static file_t * 	DRIVE_allocate_file						(file_handle_t * p_file_handle);
static void 		DRIVE_free_file							(file_t * p_file);
static file_t * 	DRIVE_file_handle_to_file_pointer		(file_handle_t file_handle);

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

static DRIVE_info_t DRIVE_info;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	Pre-kernel module initialization function
 *
 ****************************************************************************************************/
void DRIVE_init(void)
{
	// Initialize NVMCTRL module
	NVMCTRL_init();

	// Initialize EIC driver here,
	// since external interrupts are all handled in DRIVE context
	EXTI_init();

	// Initialize SD card SPI channel
	// SPI_init(SPI_CHANNEL_SD_CARD);
}

/****************************************************************************************************
 *	Top level task loop
 *
 * 	@param[in] p_params Unused
 ****************************************************************************************************/
void DRIVE_task(void * p_params)
{
	UNUSED(p_params);

	// if (FSIF_fs_init())
	// {
	// 	DRIVE_LOG_DBG("File system mounted (%s, %s)\n",
	// 		FSIF_get_volume_label(), FSIF_get_fat_subtype());
	// }

	while (1)
	{
		DRIVE_handle_message();
		vPortYield();
	}
}

/****************************************************************************************************
 *	DRIVE task main message handler
 *
 ****************************************************************************************************/
static void DRIVE_handle_message(void)
{
	ARCADIA_msg_t msg;

	if (ARCADIA_receive(&msg))
	{
		DRIVE_LOG_DBG("Received msg %s from %s\n", 
			ARCADIA_get_msg_type(msg.id), msg.b_sent_from_isr ? "ISR" : ARCADIA_get_task_name(msg.from));

		switch (msg.id)
		{
			case ARCADIA_MSG_ID_NOOP:
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

			case ARCADIA_MSG_ID_DRIVE_OPEN_FILE:
				DRIVE_handle_msg_open_file(&msg);
				break;

			case ARCADIA_MSG_ID_DRIVE_CLOSE_FILE:
				DRIVE_handle_msg_close_file(&msg);
				break;

			case ARCADIA_MSG_ID_DRIVE_CHDIR:
				DRIVE_handle_msg_chdir(&msg);
				break;

			case ARCADIA_MSG_ID_DRIVE_FETCH_FNAMES:
				DRIVE_handle_msg_fetch_fnames(&msg);
				break;

			case ARCADIA_MSG_ID_DRIVE_WRITE:
				DRIVE_handle_msg_write(&msg);
				break;

			case ARCADIA_MSG_ID_DRIVE_READ:
				DRIVE_handle_msg_read(&msg);
				break;

			case ARCADIA_MSG_ID_DRIVE_SEEK:
				DRIVE_handle_msg_seek(&msg);
				break;

			default:
				DRIVE_LOG_DBG("Unexpected message: %u\n", msg.id);
		}
	}
}

/****************************************************************************************************
 *	Blocking message handler for `ARCADIA_MSG_ID_DRIVE_READ_NVM`
 *
 * 	Reads a row of data into the provided payload's buffer
 * 
 * 	@param[in] p_msg A pointer to the received message
 *
 ****************************************************************************************************/
static void	DRIVE_handle_msg_read_nvm(ARCADIA_msg_t * p_msg)
{
	uint32_t 	u32_addr = p_msg->payload.drive_payload_read_nvm.u32_addr;
	uint8_t * 	pc_buffer = (uint8_t *)p_msg->payload.drive_payload_read_nvm.pc_buffer;

	*p_msg->payload.drive_payload_read_nvm.p_result_status = ARCADIA_STATUS_OK;

	DRIVE_LOG_DBG("Reading row (0x%08X - 0x%08X)\n", u32_addr, u32_addr + NVMCTRL_ROW_SIZE - 1);

	for (uint16_t i = 0; i < NVMCTRL_ROW_SIZE; i++)
	{
		pc_buffer[i] = ((uint8_t *)NVMCTRL_MEMORY)[u32_addr + i];
	}

	ARCADIA_semaphore_give(p_msg->semaphore);

	DRIVE_LOG_DBG("Handled msg %s with status %u\n",
				ARCADIA_get_msg_type(p_msg->id), *p_msg->payload.drive_payload_read_nvm.p_result_status);
}

/****************************************************************************************************
 *	Blocking message handler for `ARCADIA_MSG_ID_DRIVE_WRITE_NVM`
 *
 * 	Writes the data in the payload's buffer into the provided address
 * 
 * 	@param[in] p_msg A pointer to the received message
 *
 ****************************************************************************************************/
static void DRIVE_handle_msg_write_nvm(ARCADIA_msg_t * p_msg)
{
	uint32_t 	u32_addr = p_msg->payload.drive_payload_write_nvm.u32_addr;
	uint8_t * 	pc_buffer = (uint8_t *)p_msg->payload.drive_payload_write_nvm.kpc_buffer;

	DRIVE_LOG_DBG("Writing to row (0x%08X - 0x%08X)\n", u32_addr, u32_addr + NVMCTRL_ROW_SIZE - 1);

	for (uint8_t i = 0; i < (NVMCTRL_ROW_SIZE / NVMCTRL_PAGE_SIZE); i++)
	{
		NVMCTRL_write_page(u32_addr, pc_buffer + (NVMCTRL_PAGE_SIZE * i));
		u32_addr += NVMCTRL_PAGE_SIZE;
	}

	*p_msg->payload.drive_payload_write_nvm.p_result_status = ARCADIA_STATUS_OK;

	ARCADIA_semaphore_give(p_msg->semaphore);

	DRIVE_LOG_DBG("Handled msg %s with status %u\n",
				ARCADIA_get_msg_type(p_msg->id), *p_msg->payload.drive_payload_write_nvm.p_result_status);
}

/****************************************************************************************************
 *	Blocking message handler for `ARCADIA_MSG_ID_DRIVE_ERASE_NVM`
 *
 * 	Erases a row at the provided address
 * 
 * 	@param[in] p_msg A pointer to the received message
 ****************************************************************************************************/
static void DRIVE_handle_msg_erase_nvm(ARCADIA_msg_t * p_msg)
{
	uint32_t u32_addr = p_msg->payload.drive_payload_erase_nvm.u32_addr;

	DRIVE_LOG_DBG("Erasing row (0x%08X - 0x%08X)\n", u32_addr, u32_addr + NVMCTRL_ROW_SIZE - 1);

	NVMCTRL_erase_row(u32_addr);

	*p_msg->payload.drive_payload_erase_nvm.p_result_status = ARCADIA_STATUS_OK;

	ARCADIA_semaphore_give(p_msg->semaphore);

	DRIVE_LOG_DBG("Handled msg %s with status %u\n",
				ARCADIA_get_msg_type(p_msg->id), *p_msg->payload.drive_payload_erase_nvm.p_result_status);
}

/****************************************************************************************************
 *	Blocking message handler for `ARCADIA_MSG_ID_DRIVE_OPEN_FILE`
 *
 * 	Attempts to open a file with the given filename and mode.
 * 	Allocates a file handle and attempts to open the file using the specified mode.
 * 	If successful, assigns the file pointer to the message payload.
 * 	If unsuccessful, releases the allocated file structure.
 * 
 * 	@param[in] p_msg A pointer to the received message
 ****************************************************************************************************/
static void DRIVE_handle_msg_open_file(ARCADIA_msg_t * p_msg)
{
	const char * 	kpc_fname = p_msg->payload.drive_payload_open_file.kpc_fname;
	int32_t			i32_open_flag = FSIF_open_mode_from_posix_flag(p_msg->payload.drive_payload_open_file.kpc_open_mode);

	ASSERT(i32_open_flag != FSIF_INVALID_OPEN_MODE);

	file_t * 	p_file = DRIVE_allocate_file(p_msg->payload.drive_payload_open_file.p_file_handle);
	FRESULT		f_result;

	if (p_file)
	{
		f_result = f_open(p_file, kpc_fname, i32_open_flag);

		if (FR_OK == f_result)
		{
			*p_msg->payload.drive_payload_open_file.p_result_status = ARCADIA_STATUS_OK;
			DRIVE_LOG_DBG("Opened file \"%s\" with handle %d\n", kpc_fname, *p_msg->payload.drive_payload_open_file.p_file_handle);
		}
		else
		{
			DRIVE_LOG_WARN("Failed to open file (status: %u)\n", f_result);
			*p_msg->payload.drive_payload_open_file.p_result_status = ARCADIA_STATUS_FAILED;
		}
	}
	else
	{
		DRIVE_LOG_WARN("Failed to allocate a file\n");
		*p_msg->payload.drive_payload_open_file.p_result_status = ARCADIA_STATUS_FAILED;
	}
	
	ARCADIA_semaphore_give(p_msg->semaphore);

	DRIVE_LOG_DBG("Handled msg %s with status %u\n",
				ARCADIA_get_msg_type(p_msg->id), *p_msg->payload.drive_payload_open_file.p_result_status);
}

/****************************************************************************************************
 *	Blocking message handler for `ARCADIA_MSG_ID_DRIVE_CLOSE_FILE`
 *
 * 	Closes an open file and returns the file resource to the pool.
 * 
 * 	@param[in] p_msg A pointer to the received message
 ****************************************************************************************************/
static void DRIVE_handle_msg_close_file(ARCADIA_msg_t * p_msg)
{
	file_handle_t 	file_handle = p_msg->payload.drive_payload_close_file.file_handle;
	file_t * 		p_file = DRIVE_file_handle_to_file_pointer(file_handle);
	FRESULT 		f_result = f_close(p_file);

	if (FR_OK == f_result)
	{
		*p_msg->payload.drive_payload_close_file.p_result_status = ARCADIA_STATUS_OK;
		DRIVE_free_file(p_file);
		DRIVE_LOG_DBG("File %d closed\n", file_handle);
	}
	else
	{
		DRIVE_LOG_WARN("Failed to close file (status: %u)\n", f_result);
	}

	ARCADIA_semaphore_give(p_msg->semaphore);

	DRIVE_LOG_DBG("Handled msg %s with status %u\n",
				ARCADIA_get_msg_type(p_msg->id), *p_msg->payload.drive_payload_close_file.p_result_status);
}

/****************************************************************************************************
 *	Blocking message handler for `ARCADIA_MSG_ID_DRIVE_WRITE`
 *
 *	Writes the provided data to the specified file.
 *	Attempts to write the full length of the input string and verifies success by checking
 *	the number of bytes written.
 * 
 *	@param[in] p_msg A pointer to the received message
 ****************************************************************************************************/
static void	DRIVE_handle_msg_write(ARCADIA_msg_t * p_msg)
{
	file_handle_t 		file_handle = p_msg->payload.drive_payload_close_file.file_handle;
	file_t * 			p_file = DRIVE_file_handle_to_file_pointer(file_handle);
	const char * 		kpc_data = p_msg->payload.drive_payload_write.kpc_data;
	uint32_t			u32_bytes_to_write = strlen(kpc_data);
	uint32_t			u32_bytes_written = 0;
	MEMPOOL_buffer_t 	buffer = MEMPOOL_alloc(MEMPOOL_BUFFER_SIZE_ID_256);

	FRESULT f_result = f_write(p_file, kpc_data, u32_bytes_to_write, (UINT *)&u32_bytes_written);

	if (FR_OK == f_result && u32_bytes_to_write == u32_bytes_written)
	{
		DRIVE_LOG_DBG("Wrote %u bytes to file\n", u32_bytes_written);
		*p_msg->payload.drive_payload_write.p_result_status = ARCADIA_STATUS_OK;
	}
	else
	{
		DRIVE_LOG_WARN("Failed to write data to file (status: %u)\n", f_result);
		*p_msg->payload.drive_payload_write.p_result_status = ARCADIA_STATUS_FAILED;
	}

	MEMPOOL_free(buffer);

	ARCADIA_semaphore_give(p_msg->semaphore);

	DRIVE_LOG_DBG("Handled msg %s with status %u\n",
			ARCADIA_get_msg_type(p_msg->id), *p_msg->payload.drive_payload_write.p_result_status);
}

/****************************************************************************************************
 *	Blocking message handler for `ARCADIA_MSG_ID_DRIVE_READ`
 *
 *	Reads data from the file specified by the incoming message.
 *	Attempts to write the full length of the input string and verifies success by checking
 *	the number of bytes written.
 * 
 *	@param[in] p_msg A pointer to the received message
 ****************************************************************************************************/
static void	DRIVE_handle_msg_read(ARCADIA_msg_t * p_msg)
{
	file_handle_t 		file_handle = p_msg->payload.drive_payload_close_file.file_handle;
	file_t * 			p_file = DRIVE_file_handle_to_file_pointer(file_handle);
	char *		 		kpc_data = p_msg->payload.drive_payload_read.pc_data;
	uint32_t			u32_bytes_to_read = p_msg->payload.drive_payload_read.u32_bytes_to_read;
	uint32_t			u32_bytes_read = 0;

	FRESULT f_result = f_read(p_file, kpc_data, u32_bytes_to_read, (UINT *)&u32_bytes_read);

	if (FR_OK == f_result)
	{
		DRIVE_LOG_DBG("Read %u bytes from file\n", u32_bytes_read);
		*p_msg->payload.drive_payload_read.p_result_status = ARCADIA_STATUS_OK;
	}
	else
	{
		DRIVE_LOG_WARN("Failed to seek (status: %u)\n", f_result);
		*p_msg->payload.drive_payload_read.p_result_status = ARCADIA_STATUS_FAILED;
	}

	ARCADIA_semaphore_give(p_msg->semaphore);

	DRIVE_LOG_DBG("Handled msg %s with status %u\n",
			ARCADIA_get_msg_type(p_msg->id), *p_msg->payload.drive_payload_read.p_result_status);
}

/****************************************************************************************************
 *	Blocking message handler for `ARCADIA_MSG_ID_DRIVE_SEEK`
 *
 *	Seeks the internal file system's file pointer forward by the provided offset
 * 
 *	@param[in] p_msg A pointer to the received message
 ****************************************************************************************************/
static void	DRIVE_handle_msg_seek(ARCADIA_msg_t * p_msg)
{
	file_handle_t 		file_handle = p_msg->payload.drive_payload_close_file.file_handle;
	file_t * 			p_file = DRIVE_file_handle_to_file_pointer(file_handle);
	uint32_t	 		u32_offset = p_msg->payload.drive_payload_seek.u32_offset;
	uint32_t			u32_file_size = f_size(p_file);
	FRESULT				f_result;

	if (u32_offset < u32_file_size)
	{
		f_result = f_lseek(p_file, u32_offset);

		if (FR_OK == f_result)
		{
			DRIVE_LOG_DBG("Seeked forward %u bytes in file %u\n", u32_offset, file_handle);
			*p_msg->payload.drive_payload_seek.p_result_status = ARCADIA_STATUS_OK;
		}
		else
		{
			DRIVE_LOG_WARN("Failed to read data from file (status: %u)\n", f_result);
			*p_msg->payload.drive_payload_seek.p_result_status = ARCADIA_STATUS_FAILED;
		}
	}
	else
	{
		DRIVE_LOG_WARN("Out of bounds file seek request (offset: %u, file size: %u)\n", u32_offset, u32_file_size);
		*p_msg->payload.drive_payload_seek.p_result_status = ARCADIA_STATUS_DRIVE_OOB_SEEK_ERROR;
	}

	ARCADIA_semaphore_give(p_msg->semaphore);

	DRIVE_LOG_DBG("Handled msg %s with status %u\n",
			ARCADIA_get_msg_type(p_msg->id), *p_msg->payload.drive_payload_seek.p_result_status);
}

/****************************************************************************************************
 *	Blocking message handler for `ARCADIA_MSG_ID_DRIVE_CHDIR`
 *
 *	Attempts to change the current working directory to the provided path.
 *	If successful, retrieves and logs the new working directory.
 *
 *	@param[in] p_msg A pointer to the received message
 ****************************************************************************************************/
static void	DRIVE_handle_msg_chdir(ARCADIA_msg_t * p_msg)
{
	const char * kpc_dirname = p_msg->payload.drive_payload_chdir.kpc_dirname;
	MEMPOOL_buffer_t buffer = MEMPOOL_alloc(MEMPOOL_BUFFER_SIZE_ID_512);

	ASSERT(buffer);

	FRESULT f_result = f_chdir(kpc_dirname);

	if (FR_OK == f_result)
	{
		f_result = f_getcwd((TCHAR *)buffer, MEMPOOL_BUFFER_SIZE_512);

		if (FR_OK == f_result)
		{
			DRIVE_LOG_DBG("Changed to directory: %s\n", (char *)buffer);
		}
		else
		{
			DRIVE_LOG_WARN("Couldn't retrieve current directory\n");
		}

		*p_msg->payload.drive_payload_chdir.p_result_status = ARCADIA_STATUS_OK;
	}
	else
	{
		DRIVE_LOG_WARN("Failed to change directory to %s (status: %u)\n", kpc_dirname, f_result);
		*p_msg->payload.drive_payload_chdir.p_result_status = ARCADIA_STATUS_DRIVE_DIRECTORY_ERROR;
	}

	MEMPOOL_free(buffer);

	ARCADIA_semaphore_give(p_msg->semaphore);

	DRIVE_LOG_DBG("Handled msg %s with status %u\n",
			ARCADIA_get_msg_type(p_msg->id), *p_msg->payload.drive_payload_chdir.p_result_status);
}

/****************************************************************************************************
 *	Blocking message handler for `ARCADIA_MSG_ID_DRIVE_FETCH_FNAMES`
 *
 *	Searches for filenames matching a given filter.
 *
 * 	@todo
 * 	Get this to work on any directory level
 * 
 *	@param[in] p_msg A pointer to the received message
 ****************************************************************************************************/
static void	DRIVE_handle_msg_fetch_fnames(ARCADIA_msg_t * p_msg)
{
	uint8_t * 		pu8_num_found = p_msg->payload.drive_payload_fetch_fnames.pu8_num_found;
	const char * 	kpc_filter = p_msg->payload.drive_payload_fetch_fnames.kpc_filter;
	FRESULT			f_result;
	DIR				dir_obj;
	FILINFO 		f_info;

	*pu8_num_found = 0;

	// Hardcoded to top level directory
	f_result = f_findfirst(&dir_obj, &f_info, "", kpc_filter);

    while (f_result == FR_OK && f_info.fname[0])
	{
		strncpy(p_msg->payload.drive_payload_fetch_fnames.ppc_buffer[(*pu8_num_found)++], f_info.fname, COMMON_MAX_FNAME_SIZE);
        f_result = f_findnext(&dir_obj, &f_info);
    }

    f_closedir(&dir_obj);

	*p_msg->payload.drive_payload_fetch_fnames.p_result_status = ARCADIA_STATUS_OK;

	ARCADIA_semaphore_give(p_msg->semaphore);

	DRIVE_LOG_DBG("Handled msg %s with status %u\n",
				ARCADIA_get_msg_type(p_msg->id), *p_msg->payload.drive_payload_fetch_fnames.p_result_status);
}

/****************************************************************************************************
 *	Allocates an file from the file pool.
 *
 *	Searches for an unused file entry in the file pool and marks it as in use.
 *	If an available entry is found, updates the provided handle and returns a pointer to the file.
 *	If no available entry exists, returns NULL.
 * 
 *	@param[out] p_file_handle A pointer to store the allocated file handle index
 *
 *	@return A pointer to the allocated file structure, or NULL if no file is available
 ****************************************************************************************************/
static file_t * DRIVE_allocate_file(file_handle_t * p_file_handle)
{
	for (uint8_t i = 0; i < DRIVE_MAX_OPEN_FILES; ++i)
	{
		if (!DRIVE_info.file_pool[i].b_in_use)
		{
			*p_file_handle = i;
			DRIVE_info.file_pool[i].b_in_use = true;
			return &DRIVE_info.file_pool[i].file;
		}
	}

	*p_file_handle = FSIF_INVALID_FILE;

	return NULL;
}

/****************************************************************************************************
 *	Frees a previously allocated file structure.
 *
 *	Searches the file pool for the given file pointer and marks it as no longer in use.
 *	If the file is found, it is freed for future allocations.
 * 
 *	@param[in] p_file A pointer to the file structure to be freed
 ****************************************************************************************************/
static void DRIVE_free_file(file_t * p_file)
{
	for (uint8_t i = 0; i < DRIVE_MAX_OPEN_FILES; ++i)
	{
		if (&DRIVE_info.file_pool[i].file == p_file)
		{
			DRIVE_info.file_pool[i].b_in_use = false;
			return;
		}
	}
}

/****************************************************************************************************
 *	Converts a file handle to a file pointer.
 *
 *	Looks up the file associated with a given file handle. If the handle is valid
 *	and the file is in use, it returns a pointer to the corresponding file structure.
 * 
 *	@param[in] file_handle The file handle to be converted.
 *	
 *	@return A pointer to the corresponding file structure, or NULL if the handle is invalid.
 ****************************************************************************************************/
static file_t * DRIVE_file_handle_to_file_pointer(file_handle_t file_handle)
{
	if (file_handle >= DRIVE_MAX_OPEN_FILES || !DRIVE_info.file_pool[file_handle].b_in_use)
	{
		return NULL;
	}
	return &DRIVE_info.file_pool[file_handle].file;
}
