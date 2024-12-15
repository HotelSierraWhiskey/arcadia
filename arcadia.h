#ifndef ARCADIA_H
#define ARCADIA_H

#include "common.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

typedef enum _ARCADIA_task_id
{
	ARCADIA_TASK_ID_SHELL = 0,
	ARCADIA_TASK_ID_DRIVE,
	//////////
	ARCADIA_TASK_ID_NUM_IDS
} ARCADIA_task_id;

typedef enum _ARCADIA_msg_id
{
	ARCADIA_MSG_ID_NOOP = 0,
	//////////
	ARCADIA_MSG_ID_NUM_IDS
} ARCADIA_msg_id_t;

typedef struct _ARCADIA_msg
{
	ARCADIA_msg_id_t	id;
	ARCADIA_task_id		from;
} ARCADIA_msg_t;

void 				ARCADIA_start					(void);
uint32_t 			ARCADIA_send					(ARCADIA_task_id task_id, ARCADIA_msg_t * p_msg);
uint32_t 			ARCADIA_receive					(ARCADIA_msg_t * p_msg);
TaskHandle_t 		ARCADIA_handle_from_id			(ARCADIA_task_id task_id);
ARCADIA_task_id 	ARCADIA_get_current_task_id		(void);
const char *		ARCADIA_get_task_name			(ARCADIA_task_id task_id);
const char *		ARCADIA_get_msg_type			(ARCADIA_msg_id_t msg_id);


#endif // ARCADIA_H
