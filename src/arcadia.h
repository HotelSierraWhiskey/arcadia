#ifndef ARCADIA_H
#define ARCADIA_H

#include "common.h"
#include "arcadia_task.h"
#include "drive_payload.h"
#include "chrono_payload.h"
#include "load_payload.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define ARCADIA_SEMAPHORE_WAIT_MS 			(1000U)
#define ARCADIA_INVALID_TASK				(0xFF)
#define ARCADIA_semaphore_alloc(p_buffer) 	xSemaphoreCreateBinaryStatic(p_buffer)
#define ARCADIA_semaphore_free(semaphore) 	vSemaphoreDelete(semaphore)
#define ARCADIA_semaphore_take(semaphore) 	xSemaphoreTake(semaphore, pdMS_TO_TICKS(ARCADIA_SEMAPHORE_WAIT_MS))
#define ARCADIA_semaphore_give(semaphore)	xSemaphoreGive(semaphore)

typedef enum _ARCADIA_msg_id
{
	// Universal messages
	ARCADIA_MSG_ID_NOOP = 0,

	// DRIVE task messages
	ARCADIA_MSG_ID_DRIVE_READ_NVM,
	ARCADIA_MSG_ID_DRIVE_WRITE_NVM,
	ARCADIA_MSG_ID_DRIVE_ERASE_NVM,
	ARCADIA_MSG_ID_DRIVE_OPEN_FILE,
	ARCADIA_MSG_ID_DRIVE_CLOSE_FILE,
	ARCADIA_MSG_ID_DRIVE_FETCH_FNAMES,
	ARCADIA_MSG_ID_DRIVE_CHDIR,
	ARCADIA_MSG_ID_DRIVE_WRITE,
	ARCADIA_MSG_ID_DRIVE_READ,
	ARCADIA_MSG_ID_DRIVE_SEEK,
	ARCADIA_MSG_ID_DRIVE_GET_SIZE,

	// CHRONO task messages
	ARCADIA_MSG_ID_CHRONO_TIMER_ELAPSED,
	ARCADIA_MSG_ID_CHRONO_SCHEDULE_MSG_FOR_TASK,
	ARCADIA_MSG_ID_CHRONO_CANCEL_SCHEDULED_MSG,
	ARCADIA_MSG_ID_CHRONO_DEBOUNCE_EXTI,

	// LOAD task messages
	ARCADIA_MSG_ID_LOAD_LOAD_MODULE,
	//////////
	ARCADIA_MSG_ID_NUM_IDS
} ARCADIA_msg_id_t;

typedef union _ARCADIA_payload
{
	// DRIVE task payloads
	DRIVE_PAYLOAD_read_nvm_t				drive_payload_read_nvm;
	DRIVE_PAYLOAD_write_nvm_t				drive_payload_write_nvm;
	DRIVE_PAYLOAD_erase_nvm_t				drive_payload_erase_nvm;
	DRIVE_PAYLOAD_open_file_t				drive_payload_open_file;
	DRIVE_PAYLOAD_close_file_t				drive_payload_close_file;
	DRIVE_PAYLOAD_fetch_fnames_t			drive_payload_fetch_fnames;
	DRIVE_PAYLOAD_chdir_t					drive_payload_chdir;
	DRIVE_PAYLOAD_write_t					drive_payload_write;
	DRIVE_PAYLOAD_read_t					drive_payload_read;
	DRIVE_PAYLOAD_seek_t					drive_payload_seek;
	DRIVE_PAYLOAD_get_size_t				drive_payload_get_size;

	// CHRONO task payloads
	CHRONO_PAYLOAD_timer_elapsed_t			chrono_payload_timer_elapsed;
	CHRONO_PAYLOAD_schedule_msg_for_task_t	chrono_payload_schedule_msg_for_task;
	CHRONO_PAYLOAD_cancel_scheduled_msg_t	chrono_payload_cancel_scheduled_msg;
	CHRONO_PAYLOAD_debounce_exti_t			chrono_payload_debounce_exti;

	// LOAD task payloads
	LOAD_PAYLOAD_load_module_t				load_payload_load_module;
} ARCADIA_payload_t;

typedef struct _ARCADIA_msg
{
	ARCADIA_msg_id_t		id;
	ARCADIA_task_id_t		from;
	bool					b_sent_from_isr;
	ARCADIA_payload_t		payload;
	SemaphoreHandle_t		semaphore;
	StaticSemaphore_t		semaphore_buffer;
} ARCADIA_msg_t;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void 				ARCADIA_start						(void);
uint32_t 			ARCADIA_send						(ARCADIA_task_id_t task_id, ARCADIA_msg_t * p_msg);
uint32_t 			ARCADIA_send_from_isr				(ARCADIA_task_id_t task_id, ARCADIA_msg_t * p_msg);
uint32_t 			ARCADIA_receive						(ARCADIA_msg_t * p_msg);
uint32_t 			ARCADIA_receive_nb					(ARCADIA_msg_t * p_msg);
TaskHandle_t 		ARCADIA_handle_from_id				(ARCADIA_task_id_t task_id);
ARCADIA_task_id_t 	ARCADIA_get_current_task_id			(void);
uint32_t 			ARCADIA_get_task_stack_size_words	(ARCADIA_task_id_t task_id);
const char *		ARCADIA_get_task_name				(ARCADIA_task_id_t task_id);
const char *		ARCADIA_get_msg_type				(ARCADIA_msg_id_t msg_id);

#endif // ARCADIA_H
