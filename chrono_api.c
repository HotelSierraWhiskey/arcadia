#include "arcadia.h"
#include "chrono_api.h"
#include "chrono_payload.h"

TIMER_id_t CHRONO_API_schedule_msg_for_task(ARCADIA_msg_t * p_msg, ARCADIA_task_id_t task_id, uint16_t u16_delta_seconds)
{
	TIMER_id_t 			timer_id = TIMER_INVALID;

	CHRONO_PAYLOAD_schedule_msg_for_task_t	payload =
	{
		.p_msg 				= p_msg,
		.p_task_id 			= &task_id,
		.u16_delta_seconds 	= u16_delta_seconds,
		.p_timer_id			= &timer_id
	};

	ARCADIA_msg_t msg =
	{
		.payload.chrono_payload_schedule_msg_for_task = payload,
		.id 		= ARCADIA_MSG_ID_CHRONO_SCHEDULE_MSG_FOR_TASK,
		.from 		= ARCADIA_get_current_task_id(),
	};

	msg.semaphore = ARCADIA_semaphore_alloc(&msg.semaphore_buffer);

	ARCADIA_send(ARCADIA_TASK_ID_CHRONO, &msg);
	
	ARCADIA_semaphore_take(msg.semaphore);
	ARCADIA_semaphore_free(msg.semaphore);

	return timer_id;
}
