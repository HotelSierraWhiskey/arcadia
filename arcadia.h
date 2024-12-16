#ifndef ARCADIA_H
#define ARCADIA_H

#include "common.h"
#include "drive_payload.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

typedef enum _ARCADIA_task_id
{
	ARCADIA_TASK_ID_SHELL = 0,
	ARCADIA_TASK_ID_DRIVE,
	//////////
	ARCADIA_TASK_ID_NUM_IDS
} ARCADIA_task_id_t;

typedef enum _ARCADIA_msg_id
{
	ARCADIA_MSG_ID_NOOP = 0,
	ARCADIA_MSG_ID_DRIVE_READ_NVM,
	ARCADIA_MSG_ID_DRIVE_WRITE_NVM,
	ARCADIA_MSG_ID_DRIVE_ERASE_NVM,
	//////////
	ARCADIA_MSG_ID_NUM_IDS
} ARCADIA_msg_id_t;

typedef enum _ARCADIA_status
{
	ARCADIA_STATUS_OK = 0,
	ARCADIA_STATUS_FAILED,
	//////////
	ARCADIA_STATUS_NUM_STATUSES
} ARCADIA_status_t;

typedef union _ARCADIA_payload
{
	// DRIVE task payloads
	DRIVE_PAYLOAD_write_nvm_t	drive_payload_write_nvm;
	DRIVE_PAYLOAD_read_nvm_t	drive_payload_read_nvm;
	DRIVE_PAYLOAD_erase_nvm_t	drive_payload_erase_nvm;
} ARCADIA_payload_t;

typedef struct _ARCADIA_msg
{
	ARCADIA_msg_id_t		id;
	ARCADIA_task_id_t		from;
	ARCADIA_payload_t		payload;
} ARCADIA_msg_t;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void 				ARCADIA_start					(void);
uint32_t 			ARCADIA_send					(ARCADIA_task_id_t task_id, ARCADIA_msg_t * p_msg);
uint32_t 			ARCADIA_receive					(ARCADIA_msg_t * p_msg);
TaskHandle_t 		ARCADIA_handle_from_id			(ARCADIA_task_id_t task_id);
ARCADIA_task_id_t 	ARCADIA_get_current_task_id		(void);
const char *		ARCADIA_get_task_name			(ARCADIA_task_id_t task_id);
const char *		ARCADIA_get_msg_type			(ARCADIA_msg_id_t msg_id);

#endif // ARCADIA_H
