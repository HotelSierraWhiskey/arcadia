#ifndef CHRONO_PAYLOAD_H
#define CHRONO_PAYLOAD_H

#include "common.h"
#include "timer.h"
#include "arcadia_task.h"
#include "arcadia.h"

/****************************************************************************************************
 *	F O R W A R D   D E C L A R A T I O N S
 ****************************************************************************************************/

/**
 *	Forward declarations of `ARCADIA_msg_t` and `ARCADIA_task_id_t` are necessary here to resolve 
 *  dependencies between types. The `arcadia.h` header consolidates all message payload types 
 *  and relies on these types being defined beforehand to ensure proper compilation.
 */
typedef struct 	_ARCADIA_msg ARCADIA_msg_t;
typedef enum 	_ARCADIA_task_id ARCADIA_task_id_t;

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

typedef struct _CHRONO_PAYLOAD_timer_elapsed
{
	TIMER_id_t				timer_id;
} CHRONO_PAYLOAD_timer_elapsed_t;

typedef struct _CHRONO_PAYLOAD_schedule_msg_for_task
{
	ARCADIA_msg_t *			p_msg;
	ARCADIA_task_id_t	 	task_id;
	uint16_t 				u16_delta_seconds;
	TIMER_mode_t			mode;
	TIMER_id_t *			p_timer_id;
} CHRONO_PAYLOAD_schedule_msg_for_task_t;

typedef struct _CHRONO_PAYLOAD_cancel_scheduled_msg
{
	TIMER_id_t				timer_id;
	ARCADIA_status_t *		p_result_status;
} CHRONO_PAYLOAD_cancel_scheduled_msg_t;

#endif // CHRONO_PAYLOAD_H
