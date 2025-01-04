#include "drive.h"
#include "common.h"
#include "shell.h"
#include "arcadia.h"
#include "chrono.h"
#include "nvmctrl.h"
#include "utils.h"
#include "drive_payload.h"
#include "spi.h"
#include "sd.h" // remove
#include "fsif.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define DRIVE_LOG_DBG(fmt, ...)   		SHELL_printf("\r%-10s" fmt, "[DRIVE]", ##__VA_ARGS__)
#define DRIVE_LOG_WARN(fmt, ...)   		SHELL_PRINT_WARNING("\r%-10s" fmt, "[DRIVE]", ##__VA_ARGS__)

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void 	DRIVE_handle_message		(void);
static void 	DRIVE_handle_msg_read_nvm	(ARCADIA_msg_t * p_msg);
static void 	DRIVE_handle_msg_write_nvm	(ARCADIA_msg_t * p_msg);
static void 	DRIVE_handle_msg_erase_nvm	(ARCADIA_msg_t * p_msg);

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

	// if (FSIF_fs_init())
	// {
	// 	DRIVE_LOG_DBG("%s file system mounted\r\n", FSIF_get_fat_subtype());
	// 	// FSIF_get_volume_label();
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
		DRIVE_LOG_DBG("Received msg %s from %s\r\n", 
			ARCADIA_get_msg_type(msg.id), ARCADIA_get_task_name(msg.from));

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
static void	DRIVE_handle_msg_read_nvm(ARCADIA_msg_t * p_msg)
{
	uint32_t 	u32_addr = p_msg->payload.drive_payload_read_nvm.u32_addr;
	uint8_t * 	pc_buffer = (uint8_t *)p_msg->payload.drive_payload_read_nvm.pc_buffer;

	*p_msg->payload.drive_payload_read_nvm.p_result_status = ARCADIA_STATUS_OK;

	DRIVE_LOG_DBG("Reading row (0x%08X - 0x%08X)\r\n", u32_addr, u32_addr + NVMCTRL_ROW_SIZE - 1);

	for (uint16_t i = 0; i < NVMCTRL_ROW_SIZE; i++)
	{
		pc_buffer[i] = ((uint8_t *)NVMCTRL_MEMORY)[u32_addr + i];
	}

	DRIVE_LOG_DBG("Handled msg %s with status %u\r\n",
		ARCADIA_get_msg_type(p_msg->id), *p_msg->payload.drive_payload_read_nvm.p_result_status);

	ARCADIA_semaphore_give(p_msg->semaphore);
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

	DRIVE_LOG_DBG("Writing to row (0x%08X - 0x%08X)\r\n", u32_addr, u32_addr + NVMCTRL_ROW_SIZE - 1);

	for (uint8_t i = 0; i < (NVMCTRL_ROW_SIZE / NVMCTRL_PAGE_SIZE); i++)
	{
		NVMCTRL_write_page(u32_addr, pc_buffer + (NVMCTRL_PAGE_SIZE * i));
		u32_addr += NVMCTRL_PAGE_SIZE;
	}

	*p_msg->payload.drive_payload_write_nvm.p_result_status = ARCADIA_STATUS_OK;

	DRIVE_LOG_DBG("Handled msg %s with status %u\r\n",
		ARCADIA_get_msg_type(p_msg->id), *p_msg->payload.drive_payload_read_nvm.p_result_status);

	ARCADIA_semaphore_give(p_msg->semaphore);
}

/****************************************************************************************************
 *	Blocking message handler for `ARCADIA_MSG_ID_DRIVE_ERASE_NVM`
 *
 * 	Erases a row at the provided address
 * 
 * 	@param[in] p_msg A pointer to the received message
 *
 ****************************************************************************************************/
static void DRIVE_handle_msg_erase_nvm(ARCADIA_msg_t * p_msg)
{
	uint32_t 	u32_addr = p_msg->payload.drive_payload_erase_nvm.u32_addr;

	DRIVE_LOG_DBG("Erasing row (0x%08X - 0x%08X)\r\n", u32_addr, u32_addr + NVMCTRL_ROW_SIZE - 1);

	NVMCTRL_erase_row(u32_addr);

	*p_msg->payload.drive_payload_erase_nvm.p_result_status = ARCADIA_STATUS_OK;

	DRIVE_LOG_DBG("Handled msg %s with status %u\r\n",
		ARCADIA_get_msg_type(p_msg->id), *p_msg->payload.drive_payload_read_nvm.p_result_status);

	ARCADIA_semaphore_give(p_msg->semaphore);
}
