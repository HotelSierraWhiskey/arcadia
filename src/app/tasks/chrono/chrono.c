#include "chrono.h"
#include "timer.h"
#include "io.h"
#include "sys.h"
#include "shell.h"
#include "utils.h"
#include "arcadia.h"
#include "exti.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define CHRONO_LOG_DBG(fmt, ...)   					SHELL_printf("\r%-12s" fmt, "[CHRONO]", ##__VA_ARGS__)
#define CHRONO_LOG_WARN(fmt, ...)   				SHELL_PRINT_WARNING("\r%-12s" fmt, "[CHRONO]", ##__VA_ARGS__)

#define CHRONO_SYS_TICK_FREQ_1MS(source_clock_freq) ((source_clock_freq) / (1000U))

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

static CHRONO_msg_schedule_entry_t p_msg_schedule[TIMER_ID_NUM_TIMERS] =
{
	[TIMER_ID_0] =
	{
		.task_id	= ARCADIA_INVALID_TASK,
		.p_callback = NULL
	},
	[TIMER_ID_1] =
	{
		.task_id	= ARCADIA_INVALID_TASK,
		.p_callback = NULL
	},
	[TIMER_ID_2] =
	{
		.task_id	= ARCADIA_INVALID_TASK,
		.p_callback = NULL
	},
	[TIMER_ID_3] =
	{
		.task_id	= ARCADIA_INVALID_TASK,
		.p_callback = NULL
	},
};

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void 	CHRONO_systick_init							(void);
static void 	CHRONO_handle_message						(void);
static void 	CHRONO_handle_msg_timer_elapsed				(ARCADIA_msg_t * p_msg);
static void 	CHRONO_handle_msg_schedule_msg_for_task		(ARCADIA_msg_t * p_msg);
static void 	CHRONO_handle_msg_cancel_scheduled_msg		(ARCADIA_msg_t * p_msg);
static void		CHRONO_handle_msg_debounce_exti				(ARCADIA_msg_t * p_msg);

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void vPortSetupTimerInterrupt(void)
{
	CHRONO_systick_init();
}

static void CHRONO_systick_init(void)
{
	uint32_t u32_source_clock_freq = SYS_get_source_clock_freq();
	SysTick_Config(CHRONO_SYS_TICK_FREQ_1MS(u32_source_clock_freq));
}

void CHRONO_init(void)
{
	TIMER_init();
}

void CHRONO_task(void * p_params)
{
	UNUSED(p_params);

	while (1)
	{
		CHRONO_handle_message();
		vPortYield();
	}
}

CHRONO_msg_schedule_entry_t	* CHRONO_get_msg_schedule_entry(TIMER_id_t timer_id)
{
	ASSERT(timer_id < TIMER_ID_NUM_TIMERS);
	return &p_msg_schedule[timer_id];
}

/****************************************************************************************************
 *	CHRONO task main message handler
 *
 ****************************************************************************************************/
static void CHRONO_handle_message(void)
{
	ARCADIA_msg_t msg;

	if (ARCADIA_receive(&msg))
	{
		CHRONO_LOG_DBG("Received msg %s from %s\n", 
			ARCADIA_get_msg_type(msg.id), msg.b_sent_from_isr ? "ISR" : ARCADIA_get_task_name(msg.from));

		switch (msg.id)
		{
			case ARCADIA_MSG_ID_NOOP:
				break;

			case ARCADIA_MSG_ID_CHRONO_TIMER_ELAPSED:
				CHRONO_handle_msg_timer_elapsed(&msg);
				break;

			case ARCADIA_MSG_ID_CHRONO_SCHEDULE_MSG_FOR_TASK:
				CHRONO_handle_msg_schedule_msg_for_task(&msg);
				break;

			case ARCADIA_MSG_ID_CHRONO_CANCEL_SCHEDULED_MSG:
				CHRONO_handle_msg_cancel_scheduled_msg(&msg);
				break;

			case ARCADIA_MSG_ID_CHRONO_DEBOUNCE_EXTI:
				CHRONO_handle_msg_debounce_exti(&msg);
				break;
			
			default:
				CHRONO_LOG_DBG("Unexpected message: %u\n", msg.id);
		}
	}
}

static void	CHRONO_handle_msg_timer_elapsed(ARCADIA_msg_t * p_msg)
{
	TIMER_id_t 						timer_id = p_msg->payload.chrono_payload_timer_elapsed.timer_id;
	CHRONO_msg_schedule_entry_t	* 	p_sched_entry = &p_msg_schedule[timer_id];
	ARCADIA_task_id_t 				task_id = p_sched_entry->task_id;
	ARCADIA_msg_t * 				p_scheduled_msg = &p_sched_entry->msg;
	const TIMER_info_t * 			kp_timer_info = TIMER_get_timer_info(timer_id);

	// Don't self-block
	if (task_id != ARCADIA_INVALID_TASK && task_id != ARCADIA_TASK_ID_CHRONO)
	{
		CHRONO_LOG_DBG("Timer %u Elapsed. Relaying msg %s to task %s\n", 
			timer_id, ARCADIA_get_msg_type(p_sched_entry->msg.id), ARCADIA_get_task_name(task_id));
		
		ARCADIA_send(task_id, p_scheduled_msg);

		// This semaphore was allocated when the message was copied into the schedule
		ARCADIA_semaphore_take(p_sched_entry->msg.semaphore);
		ARCADIA_semaphore_free(p_sched_entry->msg.semaphore);

		// Clean up the schedule slot that was used, if applicable
		if (TIMER_MODE_SINGLE_SHOT == kp_timer_info->mode)
		{
			memset(&p_sched_entry->msg, 0, sizeof(ARCADIA_msg_t));
			p_sched_entry->task_id = ARCADIA_INVALID_TASK;
		}
	}
	if (p_sched_entry->p_callback)
	{
		p_sched_entry->p_callback(p_sched_entry->u32_callback_args);
		p_sched_entry->p_callback = NULL;
		p_sched_entry->u32_callback_args = 0;
	}

	CHRONO_LOG_DBG("Handled msg %s\n", ARCADIA_get_msg_type(p_msg->id));
}

static void CHRONO_handle_msg_schedule_msg_for_task(ARCADIA_msg_t * p_msg)
{
	uint16_t 		u16_period = p_msg->payload.chrono_payload_schedule_msg_for_task.u64_delta_ms;
	TIMER_mode_t	mode = p_msg->payload.chrono_payload_schedule_msg_for_task.mode;
	TIMER_id_t 		timer_id = TIMER_alloc(u16_period, mode);

	if (timer_id != TIMER_INVALID)
	{
		// All is well. Populate the schedule with the message to be sent and save the task to whom this msg is addressed.
		// Deep copy the msg and payload. We can't take a reference from the stack. Allocate the semaphore here too.
		memcpy(&p_msg_schedule[timer_id].msg, p_msg->payload.chrono_payload_schedule_msg_for_task.p_msg, sizeof(ARCADIA_msg_t));
		memcpy(&p_msg_schedule[timer_id].msg.payload, &p_msg->payload.chrono_payload_schedule_msg_for_task.p_msg->payload, sizeof(ARCADIA_payload_t));

		p_msg_schedule[timer_id].task_id = p_msg->payload.chrono_payload_schedule_msg_for_task.task_id;
		p_msg_schedule[timer_id].msg.semaphore = ARCADIA_semaphore_alloc(&p_msg_schedule[timer_id].msg.semaphore_buffer);

		TIMER_start(timer_id);

		CHRONO_LOG_DBG("Message %s scheduled for transmission to %s in %u seconds\n",
			ARCADIA_get_msg_type(p_msg_schedule[timer_id].msg.id),
			ARCADIA_get_task_name(p_msg_schedule[timer_id].task_id),
			p_msg->payload.chrono_payload_schedule_msg_for_task.u64_delta_ms);
	}
	else
	{
		CHRONO_LOG_WARN("Timer pool empty. Message scheduling failed\n");
	}

	ARCADIA_semaphore_give(p_msg->semaphore);
}

static void CHRONO_handle_msg_cancel_scheduled_msg (ARCADIA_msg_t * p_msg)
{
	TIMER_id_t 						timer_id = p_msg->payload.chrono_payload_cancel_scheduled_msg.timer_id;
	CHRONO_msg_schedule_entry_t *	p_schedule_entry = &p_msg_schedule[timer_id];

	*(p_msg->payload.chrono_payload_cancel_scheduled_msg.p_result_status) = ARCADIA_STATUS_OK;

	if (p_schedule_entry->task_id != ARCADIA_INVALID_TASK)
	{
		TIMER_stop(timer_id);

		CHRONO_LOG_DBG("Cancelled schedule for msg %s to %s\n",
			ARCADIA_get_msg_type(p_msg_schedule[timer_id].msg.id),
			ARCADIA_get_task_name(p_msg_schedule[timer_id].task_id));
		
		p_schedule_entry->task_id = ARCADIA_INVALID_TASK;
		memset(&p_schedule_entry->msg, 0, sizeof(ARCADIA_msg_t));
	}
	else
	{
		CHRONO_LOG_WARN("Nothing scheduled on timer %u\n", timer_id);
	}

	ARCADIA_semaphore_give(p_msg->semaphore);
}

static void CHRONO_handle_msg_debounce_exti (ARCADIA_msg_t * p_msg)
{
	TIMER_id_t timer_id = TIMER_alloc(EXTI_DEBOUNCE_MS, TIMER_MODE_SINGLE_SHOT);

	if (timer_id != TIMER_INVALID)
	{
		p_msg_schedule[timer_id].p_callback = EXTI_update;
		p_msg_schedule[timer_id].u32_callback_args = p_msg->payload.chrono_payload_debounce_exti.u32_sources;
	}							
	
	TIMER_start(timer_id);
}

uint32_t CHRONO_ticks_since(uint32_t start_ticks)
{
	return CHRONO_get_ticks() - start_ticks;
}
