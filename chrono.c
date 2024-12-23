#include "chrono.h"
#include "timer.h"
#include "io.h"
#include "sys.h"
#include "shell.h"
#include "utils.h"
#include "arcadia.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define CHRONO_LOG_DBG(fmt, ...)   					SHELL_printf("%-10s" fmt, "[CHRONO]", ##__VA_ARGS__)

#define CHRONO_SYS_TICK_FREQ_1MS(source_clock_freq) ((source_clock_freq) / (1000U))

typedef struct _CHRONO_msg_schedule_entry
{
	ARCADIA_task_id_t	task_id;
	ARCADIA_msg_t		msg;
} CHRONO_msg_schedule_entry_t;

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

static CHRONO_msg_schedule_entry_t p_msg_schedule[TIMER_ID_NUM_TIMERS] =
{
	[TIMER_ID_0] =
	{
		.task_id	= ARCADIA_INVALID_TASK
	},
	[TIMER_ID_1] =
	{
		.task_id	= ARCADIA_INVALID_TASK
	},
	[TIMER_ID_2] =
	{
		.task_id	= ARCADIA_INVALID_TASK
	},
	[TIMER_ID_3] =
	{
		.task_id	= ARCADIA_INVALID_TASK
	},
};

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void 	CHRONO_systick_init							(void);
static void 	CHRONO_handle_message						(void);
static void 	CHRONO_handle_msg_timer_elapsed				(ARCADIA_msg_t * p_msg);
static void 	CHRONO_handle_msg_schedule_msg_for_task		(ARCADIA_msg_t * p_msg);

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

/****************************************************************************************************
 *	CHRONO task main message handler
 *
 ****************************************************************************************************/
static void CHRONO_handle_message(void)
{
	ARCADIA_msg_t msg;

	if (ARCADIA_receive(&msg))
	{
		CHRONO_LOG_DBG("Received msg %s from %s\r\n", 
			ARCADIA_get_msg_type(msg.id), ARCADIA_get_task_name(msg.from));

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
			
			default:
				CHRONO_LOG_DBG("Unexpected message: %u\r\n", msg.id);
		}
	}
}

static void	CHRONO_handle_msg_timer_elapsed(ARCADIA_msg_t * p_msg)
{
	TIMER_id_t timer_id = p_msg->payload.chrono_payload_timer_elapsed.timer_id;
	ARCADIA_task_id_t task_id = p_msg_schedule[timer_id].task_id;

	CHRONO_LOG_DBG("Relaying msg %s to task %s\r\n", 
		ARCADIA_get_msg_type(p_msg_schedule[timer_id].msg.id), ARCADIA_get_task_name(task_id));
	
	ARCADIA_send(task_id, &p_msg_schedule[timer_id].msg);

	// This semaphore was allocated when the message was copied into the schedule
	ARCADIA_semaphore_take(p_msg_schedule[timer_id].msg.semaphore);
	ARCADIA_semaphore_free(p_msg_schedule[timer_id].msg.semaphore);

	// Clean up the schedule slot that was used
	memset(&p_msg_schedule[timer_id].msg, 0, sizeof(ARCADIA_msg_t));
	p_msg_schedule[timer_id].task_id = ARCADIA_INVALID_TASK;

	CHRONO_LOG_DBG("Timer %u Elapsed\r\n", timer_id);
	CHRONO_LOG_DBG("Handled msg %s\r\n", ARCADIA_get_msg_type(p_msg->id));
}

static void CHRONO_handle_msg_schedule_msg_for_task(ARCADIA_msg_t * p_msg)
{
	uint16_t u16_period = p_msg->payload.chrono_payload_schedule_msg_for_task.u16_delta_seconds;
	TIMER_id_t timer_id = TIMER_alloc(u16_period, 0); // tmp 0

	if (timer_id != TIMER_INVALID)
	{
		// Deep copy the msg into the registry. We can't take a reference from the stack. Allocate the semaphore here too.
		memcpy(&p_msg_schedule[timer_id].msg, p_msg->payload.chrono_payload_schedule_msg_for_task.p_msg, sizeof(ARCADIA_msg_t));
		p_msg_schedule[timer_id].msg.semaphore = ARCADIA_semaphore_alloc(&p_msg_schedule[timer_id].msg.semaphore_buffer);

		p_msg_schedule[timer_id].task_id = *(p_msg->payload.chrono_payload_schedule_msg_for_task.p_task_id);

		TIMER_start(timer_id);

		CHRONO_LOG_DBG("Message %s scheduled for transmission to %s in %u seconds\r\n",
			ARCADIA_get_msg_type(p_msg->payload.chrono_payload_schedule_msg_for_task.p_msg->id),
			ARCADIA_get_task_name(*(p_msg->payload.chrono_payload_schedule_msg_for_task.p_task_id)),
			p_msg->payload.chrono_payload_schedule_msg_for_task.u16_delta_seconds);
	}
	else
	{
		CHRONO_LOG_DBG("Fuck\r\n");
	}


	ARCADIA_semaphore_give(p_msg->semaphore);
}

uint32_t CHRONO_ticks_since(uint32_t start_ticks)
{
	return CHRONO_get_ticks() - start_ticks;
}
