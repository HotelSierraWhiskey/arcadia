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

void 			ARCADIA_start		(void);
uint32_t 		ARCADIA_send		(ARCADIA_task_id task_id, const void * kp_item, TickType_t ticks_to_wait);
uint32_t 		ARCADIA_receive		(ARCADIA_task_id task_id, void * p_buffer, TickType_t ticks_to_wait);

#endif // ARCADIA_H