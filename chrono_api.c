#include "arcadia.h"
#include "chrono_api.h"
#include "chrono_payload.h"
#include "utils.h"
#include "chrono.h"

TIMER_id_t CHRONO_API_schedule_msg_for_task(ARCADIA_msg_t * p_msg, ARCADIA_task_id_t task_id, uint16_t u16_delta_seconds, TIMER_mode_t mode)
{
	TIMER_id_t 			timer_id = TIMER_INVALID;
	ARCADIA_msg_t		payload_msg;

	// It's possible for the memory referenced by `p_msg` to become invalid between sending this
	// API message to CHRONO and when CHRONO copies the payload message into its schedule. 
	// Let's copy it here to guarantee its availability throughout the scheduling sequence.
	memcpy(&payload_msg, p_msg, sizeof(ARCADIA_msg_t));

	// This is the payload that will be used for scheduling by the CHRONO task
	CHRONO_PAYLOAD_schedule_msg_for_task_t	payload =
	{
		.p_msg 				= &payload_msg,
		.task_id 			= task_id,
		.u16_delta_seconds 	= u16_delta_seconds,
		.p_timer_id			= &timer_id,
		.mode				= mode
	};

	// This is the actual API message
	ARCADIA_msg_t msg =
	{
		.id 		= ARCADIA_MSG_ID_CHRONO_SCHEDULE_MSG_FOR_TASK,
		.from 		= ARCADIA_get_current_task_id(),
		.payload.chrono_payload_schedule_msg_for_task = payload,
	};

	msg.semaphore = ARCADIA_semaphore_alloc(&msg.semaphore_buffer);

	ARCADIA_send(ARCADIA_TASK_ID_CHRONO, &msg);

	ARCADIA_semaphore_take(msg.semaphore);
	ARCADIA_semaphore_free(msg.semaphore);

	return timer_id;
}

ARCADIA_status_t CHRONO_API_cancel_scheduled_message(TIMER_id_t timer_id)
{
	ASSERT(timer_id < TIMER_ID_NUM_TIMERS);

	ARCADIA_status_t status = ARCADIA_STATUS_FAILED;

	CHRONO_PAYLOAD_cancel_scheduled_msg_t payload =
	{
		.timer_id 			= timer_id,
		.p_result_status 	= &status
	};

	ARCADIA_msg_t msg =
	{
		.id = ARCADIA_MSG_ID_CHRONO_CANCEL_SCHEDULED_MSG,
		.from = ARCADIA_get_current_task_id(),
		.payload.chrono_payload_cancel_scheduled_msg = payload
	};

	msg.semaphore = ARCADIA_semaphore_alloc(&msg.semaphore_buffer);

	ARCADIA_send(ARCADIA_TASK_ID_CHRONO, &msg);

	ARCADIA_semaphore_take(msg.semaphore);
	ARCADIA_semaphore_free(msg.semaphore);

	return status;
}

/****************************************************************************************************
 *	Shell utility
 *
 * 	Schedules a NOOP msg for the given task
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t CHRONO_API_shell_sn(uint8_t argc, char ** argv)
{
	bool 				b_res = false;
	uint32_t 			task_id;
	uint32_t 			delay;
	uint32_t			mode;
	ARCADIA_msg_t		msg =
	{
		.id 	= ARCADIA_MSG_ID_NOOP,
		.from 	= ARCADIA_get_current_task_id()
	};

	if (argc == 3)
	{
		if (UTILS_string_to_u32(argv[0], &task_id))
		{
			if (task_id < ARCADIA_TASK_ID_NUM_IDS)
			{
				b_res = true;
			}
		}

		if (b_res && UTILS_string_to_u32(argv[1], &delay))
		{
			if (delay > UINT16_MAX)
			{
				b_res = false;
			}
		}
		if (b_res && UTILS_string_to_u32(argv[2], &mode))
		{
			if (mode > TIMER_MODE_NUM_MODES)
			{
				b_res = false;
			}
		}
	}
	
	if (b_res)
	{
		if (TIMER_INVALID == CHRONO_API_schedule_msg_for_task(&msg, task_id, delay, mode))
		{
			SHELL_printf("Timer invalid\r\n");
		}
	}
	else
	{
		SHELL_printf("Usage: chrono sn <task_id> <delay> <mode>\r\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

/****************************************************************************************************
 *	Shell utility
 *
 * 	Cancels a scheduled msg
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t CHRONO_API_shell_cancel(uint8_t argc, char ** argv)
{
	bool 				b_res = false;
	uint32_t 			timer_id;

	if (argc == 1)
	{
		if (UTILS_string_to_u32(argv[0], &timer_id))
		{
			if (timer_id < TIMER_ID_NUM_TIMERS)
			{
				b_res = true;
			}
		}
	}
	
	if (b_res)
	{
		if (CHRONO_API_cancel_scheduled_message(timer_id) != ARCADIA_STATUS_OK)
		{
			SHELL_printf("Unable to cancel a scheduled message on timer %u\r\n", timer_id);
		}
	}
	else
	{
		SHELL_printf("Usage: chrono cancel <timer_id>\r\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

uint8_t CHRONO_API_shell_info(uint8_t argc, char ** argv)
{
	TIMER_info_t * 					p_timer_info;
	CHRONO_msg_schedule_entry_t *	p_msg_schedule_entry;
	uint32_t 						u32_current_count;
	uint32_t 						u32_time_remaining;
	uint32_t 						u32_hours;
	uint32_t 						u32_minutes;
	uint32_t 						u32_seconds;
	char 							pc_time_buffer[16] = { 0 };

	if (argc == 0)
	{
		SHELL_SEPARATOR();

		for (uint8_t i = 0; i < TIMER_ID_NUM_TIMERS; i++)
		{
			p_timer_info 			= (TIMER_info_t *)TIMER_get_timer_info(i);
			p_msg_schedule_entry 	= CHRONO_get_msg_schedule_entry(i);

			SHELL_printf("Slot %u\r\n", i);

			if (TIMER_AVAILABLE == p_timer_info->u16_period)
			{
				SHELL_printf("\t%-20s: Unscheduled\r\n", "Status");
			}
			else
			{
				u32_current_count = TIMER_get_timer_count(i) / TIMER_PRESCALED_SECOND_COUNT_VALUE;

				u32_time_remaining = p_timer_info->u16_period - u32_current_count;

				u32_hours = u32_time_remaining / 3600;
				u32_minutes = (u32_time_remaining % 3600) / 60;
				u32_seconds = u32_time_remaining % 60;

				snprintf(pc_time_buffer, sizeof(pc_time_buffer), "%02lu:%02lu:%02lu", u32_hours, u32_minutes, u32_seconds);

				SHELL_printf("\t%-20s: Scheduled\r\n", "Status");
				SHELL_printf("\t%-20s: %s\r\n", "Msg", ARCADIA_get_msg_type(p_msg_schedule_entry->msg.id));
				SHELL_printf("\t%-20s: %s\r\n", "Addressed Task", ARCADIA_get_task_name(p_msg_schedule_entry->task_id));
				SHELL_printf("\t%-20s: %s\r\n", "From", ARCADIA_get_task_name(p_msg_schedule_entry->msg.from));
				SHELL_printf("\t%-20s: %s\r\n", "Time Remaining", pc_time_buffer);
			}
			if (i < TIMER_ID_NUM_TIMERS - 1)
			{
				SHELL_printf("\r\n");
			}
		}

		SHELL_SEPARATOR();
	}
	else
	{
		SHELL_printf("Usage: chrono info\r\n");
	}

	return SHELL_COMMAND_SUCCESS;
}
