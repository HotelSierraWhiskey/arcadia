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

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define DRIVE_LOG_DBG(fmt, ...)   		SHELL_printf("\r%-10s" fmt, "[DRIVE]", ##__VA_ARGS__)
#define DRIVE_LOG_WARN(fmt, ...)   		SHELL_PRINT_WARNING("\r%-10s" fmt, "[DRIVE]", ##__VA_ARGS__)

#define DRIVE_MAX_OPEN_FILES			(3U)

typedef struct _DRIVE_file_slot
{
	file_t 	file;
	bool 	b_in_use;
} DRIVE_file_slot_t;
typedef struct _DRIVE_info
{
	DRIVE_file_slot_t file_pool[DRIVE_MAX_OPEN_FILES];
} DRIVE_info_t;

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

static DRIVE_info_t DRIVE_info;

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void 		DRIVE_handle_message			(void);
static bool 		DRIVE_handle_msg_read_nvm		(ARCADIA_msg_t * p_msg);
static bool 		DRIVE_handle_msg_write_nvm		(ARCADIA_msg_t * p_msg);
static bool 		DRIVE_handle_msg_erase_nvm		(ARCADIA_msg_t * p_msg);

static bool 		DRIVE_handle_msg_open_file		(ARCADIA_msg_t * p_msg);
static bool 		DRIVE_handle_msg_close_file		(ARCADIA_msg_t * p_msg);

static file_t * 	DRIVE_allocate_file				(int8_t * pi8_handle);
static void 		DRIVE_free_file					(file_t * p_file);

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

	// Initialize SD card SPI channel
	SPI_init(SPI_CHANNEL_SD_CARD);
}

/****************************************************************************************************
 *	Top level task loop
 *
 ****************************************************************************************************/
void DRIVE_task(void * p_params)
{
	UNUSED(p_params);

	if (FSIF_fs_init())
	{
		DRIVE_LOG_DBG("File system mounted (%s, %s)\r\n",
			FSIF_get_volume_label(), FSIF_get_fat_subtype());
	}

	while (1)
	{
		DRIVE_handle_message();
		vPortYield();
	}
}

file_t * DRIVE_index_to_file_pointer(uint8_t u8_index)
{
	if (u8_index >= DRIVE_MAX_OPEN_FILES || !DRIVE_info.file_pool[u8_index].b_in_use)
	{
		return NULL;
	}
	return &DRIVE_info.file_pool[u8_index].file;
}

int8_t DRIVE_file_pointer_to_index(file_t *p_file)
{
	for (uint8_t i = 0; i < DRIVE_MAX_OPEN_FILES; ++i)
	{
		if (&DRIVE_info.file_pool[i].file == p_file && DRIVE_info.file_pool[i].b_in_use)
		{
			return i;
		}
	}

	return -1;
}

/****************************************************************************************************
 *	DRIVE task main message handler
 *
 ****************************************************************************************************/
static void DRIVE_handle_message(void)
{
	ARCADIA_msg_t 	msg;
	bool 			b_handled = false;

	if (ARCADIA_receive(&msg))
	{
		DRIVE_LOG_DBG("Received msg %s from %s\r\n", 
			ARCADIA_get_msg_type(msg.id), ARCADIA_get_task_name(msg.from));

		switch (msg.id)
		{
			case ARCADIA_MSG_ID_NOOP:
				break;

			case ARCADIA_MSG_ID_DRIVE_READ_NVM:
				b_handled = DRIVE_handle_msg_read_nvm(&msg);
				break;

			case ARCADIA_MSG_ID_DRIVE_WRITE_NVM:
				b_handled = DRIVE_handle_msg_write_nvm(&msg);
				break;

			case ARCADIA_MSG_ID_DRIVE_ERASE_NVM:
				b_handled = DRIVE_handle_msg_erase_nvm(&msg);
				break;

			case ARCADIA_MSG_ID_DRIVE_OPEN_FILE:
				b_handled = DRIVE_handle_msg_open_file(&msg);
				break;

			case ARCADIA_MSG_ID_DRIVE_CLOSE_FILE:
				b_handled = DRIVE_handle_msg_close_file(&msg);
				break;
			
			default:
				DRIVE_LOG_DBG("Unexpected message: %u\r\n", msg.id);
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
static bool	DRIVE_handle_msg_read_nvm(ARCADIA_msg_t * p_msg)
{
	uint32_t 	u32_addr = p_msg->payload.drive_payload_read_nvm.u32_addr;
	uint8_t * 	pc_buffer = (uint8_t *)p_msg->payload.drive_payload_read_nvm.pc_buffer;

	*p_msg->payload.drive_payload_read_nvm.p_result_status = ARCADIA_STATUS_OK;

	DRIVE_LOG_DBG("Reading row (0x%08X - 0x%08X)\r\n", u32_addr, u32_addr + NVMCTRL_ROW_SIZE - 1);

	for (uint16_t i = 0; i < NVMCTRL_ROW_SIZE; i++)
	{
		pc_buffer[i] = ((uint8_t *)NVMCTRL_MEMORY)[u32_addr + i];
	}

	ARCADIA_semaphore_give(p_msg->semaphore);

	DRIVE_LOG_DBG("Handled msg %s with status %u\r\n",
				ARCADIA_get_msg_type(p_msg->id), *p_msg->payload.drive_payload_read_nvm.p_result_status);

	return true;
}

/****************************************************************************************************
 *	Blocking message handler for `ARCADIA_MSG_ID_DRIVE_WRITE_NVM`
 *
 * 	Writes the data in the payload's buffer into the provided address
 * 
 * 	@param[in] p_msg A pointer to the received message
 *
 ****************************************************************************************************/
static bool DRIVE_handle_msg_write_nvm(ARCADIA_msg_t * p_msg)
{
	uint32_t 	u32_addr = p_msg->payload.drive_payload_write_nvm.u32_addr;
	uint8_t * 	pc_buffer = (uint8_t *)p_msg->payload.drive_payload_write_nvm.kpc_buffer;

	DRIVE_LOG_DBG("Writing to row (0x%08X - 0x%08X)\r\n", u32_addr, u32_addr + NVMCTRL_ROW_SIZE - 1);

	for (uint8_t i = 0; i < (NVMCTRL_ROW_SIZE / NVMCTRL_PAGE_SIZE); i++)
	{
		NVMCTRL_write_page(u32_addr, pc_buffer + (NVMCTRL_PAGE_SIZE * i));
		u32_addr += NVMCTRL_PAGE_SIZE;
	}

	*p_msg->payload.drive_payload_write_nvm.p_result_status = ARCADIA_STATUS_OK;

	ARCADIA_semaphore_give(p_msg->semaphore);

	DRIVE_LOG_DBG("Handled msg %s with status %u\r\n",
				ARCADIA_get_msg_type(p_msg->id), *p_msg->payload.drive_payload_write_nvm.p_result_status);

	return true;
}

/****************************************************************************************************
 *	Blocking message handler for `ARCADIA_MSG_ID_DRIVE_ERASE_NVM`
 *
 * 	Erases a row at the provided address
 * 
 * 	@param[in] p_msg A pointer to the received message
 *
 ****************************************************************************************************/
static bool DRIVE_handle_msg_erase_nvm(ARCADIA_msg_t * p_msg)
{
	uint32_t u32_addr = p_msg->payload.drive_payload_erase_nvm.u32_addr;

	DRIVE_LOG_DBG("Erasing row (0x%08X - 0x%08X)\r\n", u32_addr, u32_addr + NVMCTRL_ROW_SIZE - 1);

	NVMCTRL_erase_row(u32_addr);

	*p_msg->payload.drive_payload_erase_nvm.p_result_status = ARCADIA_STATUS_OK;

	ARCADIA_semaphore_give(p_msg->semaphore);

	DRIVE_LOG_DBG("Handled msg %s with status %u\r\n",
				ARCADIA_get_msg_type(p_msg->id), *p_msg->payload.drive_payload_erase_nvm.p_result_status);

	return true;
}

static bool DRIVE_handle_msg_open_file(ARCADIA_msg_t * p_msg)
{
	const char * 	kpc_fname = p_msg->payload.drive_payload_open_file.kpc_fname;
	int32_t			i32_open_flag = FSIF_open_mode_from_posix_flag(p_msg->payload.drive_payload_open_file.kpc_open_mode);

	ASSERT(i32_open_flag != FSIF_INVALID_OPEN_MODE);

	int8_t 		i8_handle = -1;
	file_t *	p_file = DRIVE_allocate_file(&i8_handle);
	FRESULT		f_result = f_open(p_file, kpc_fname, i32_open_flag);

	if (FR_OK == f_result)
	{
		p_msg->payload.drive_payload_open_file.p_file = p_file;
		*p_msg->payload.drive_payload_open_file.p_result_status = ARCADIA_STATUS_OK;
		DRIVE_LOG_DBG("Opened file %s with handle %d\n", kpc_fname, i8_handle);
	}
	else
	{
		DRIVE_LOG_WARN("Failed to open file (status: %u)\n", f_result);
	}

	ARCADIA_semaphore_give(p_msg->semaphore);

	DRIVE_LOG_DBG("Handled msg %s with status %u\r\n",
				ARCADIA_get_msg_type(p_msg->id), *p_msg->payload.drive_payload_open_file.p_result_status);

	return true;
}

static bool DRIVE_handle_msg_close_file(ARCADIA_msg_t * p_msg)
{
	file_t * 	p_file = p_msg->payload.drive_payload_close_file.p_file;
	int8_t 		i8_handle = DRIVE_file_pointer_to_index(p_file);
	
	ASSERT(i8_handle >= 0);
	
	FRESULT 	f_result = f_close(p_file);

	if (FR_OK == f_result)
	{
		*p_msg->payload.drive_payload_close_file.p_result_status = ARCADIA_STATUS_OK;
		DRIVE_free_file(p_file);
		DRIVE_LOG_DBG("File %d closed\n", i8_handle);
	}
	else
	{
		DRIVE_LOG_WARN("Failed to close file (status: %u)\n", f_result);
	}

	ARCADIA_semaphore_give(p_msg->semaphore);

	DRIVE_LOG_DBG("Handled msg %s with status %u\r\n",
				ARCADIA_get_msg_type(p_msg->id), *p_msg->payload.drive_payload_close_file.p_result_status);

	return true;
}

static file_t * DRIVE_allocate_file(int8_t * pi8_handle)
{
	for (uint8_t i = 0; i < DRIVE_MAX_OPEN_FILES; ++i)
	{
		if (!DRIVE_info.file_pool[i].b_in_use)
		{
			*pi8_handle = i;
			DRIVE_info.file_pool[i].b_in_use = true;
			return &DRIVE_info.file_pool[i].file;
		}
	}
	return NULL;
}

static void DRIVE_free_file(file_t *p_file)
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
