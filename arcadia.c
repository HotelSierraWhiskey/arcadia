#include "arcadia.h"
#include "common.h"
#include "FreeRTOSConfig.h"
#include "utils.h"
#include "shell.h"
#include "drive.h"
#include "chrono.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define ARCADIA_Q_LENGTH		(5U)
#define ARCADIA_Q_ITEM_SIZE		sizeof(ARCADIA_msg_t)

#define ARCADIA_SHELL_STACK_SIZE		BYTES_TO_WORDS(2048U)
#define ARCADIA_DRIVE_STACK_SIZE		BYTES_TO_WORDS(2048U)
#define ARCADIA_CHRONO_STACK_SIZE		BYTES_TO_WORDS(1024U)

/**
 *	Main task loop function pointer prototype
 */
typedef void (* ARCADIA_task_t)(void *);

/**
 *	Pre-kernel task initialization function pointer prototype
 */
typedef void (* ARCADIA_task_init_t)(void);


typedef struct _ARCADIA_stack
{
	StackType_t *	p_stack;
	uint32_t		u32_words;
} ARCADIA_stack_t;

/**
 *	Task typedef
 *
 * 	@note
 * 	Only the task's id, name, and two functions need to be provided.
 * 	The rest of the members are populated during initialization
 */
typedef struct _ARCADIA_rtos_task
{
	ARCADIA_task_id_t		task_id;
	const char * const		kpc_name;
	ARCADIA_task_t			task;
	ARCADIA_task_init_t		init;
	ARCADIA_stack_t			stack;
	StaticTask_t			task_control_block;
	TaskHandle_t			handle;

	StaticQueue_t			queue;
	uint8_t					u8_queue_storage[ARCADIA_Q_LENGTH * ARCADIA_Q_ITEM_SIZE];
	QueueHandle_t			queue_handle;
} ARCADIA_rtos_task_t;

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

/**
 *	SHELL stack
 */
StackType_t shell_stack[ARCADIA_SHELL_STACK_SIZE];

/**
 *	DRIVE stack
 */
StackType_t drive_stack[ARCADIA_DRIVE_STACK_SIZE];

/**
 *	CHRONO stack
 */
StackType_t chrono_stack[ARCADIA_CHRONO_STACK_SIZE];

/**
 *	Main RTOS task table
 */
static ARCADIA_rtos_task_t rtos_tasks[ARCADIA_TASK_ID_NUM_IDS] =
{
	{
		.task_id 	= ARCADIA_TASK_ID_SHELL,
		.kpc_name 	= "SHELL",
		.stack 		=
		{
			.p_stack 	= shell_stack,
			.u32_words 	= ARCADIA_SHELL_STACK_SIZE
		},
		.task 		= SHELL_task,
		.init		= SHELL_init
	},
	{
		.task_id 	= ARCADIA_TASK_ID_DRIVE,
		.kpc_name 	= "DRIVE",
		.stack 		=
		{
			.p_stack 	= drive_stack,
			.u32_words 	= ARCADIA_DRIVE_STACK_SIZE
		},
		.task 		= DRIVE_task,
		.init		= DRIVE_init
	},
	{
		.task_id 	= ARCADIA_TASK_ID_CHRONO,
		.kpc_name 	= "CHRONO",
		.stack 		= 
		{
			.p_stack 	= chrono_stack,
			.u32_words 	= ARCADIA_CHRONO_STACK_SIZE
		},
		.task 		= CHRONO_task,
		.init		= CHRONO_init
	},
};

/**
 *	Message descriptors
 */
static const char * const kpc_msg_descriptors[ARCADIA_MSG_ID_NUM_IDS] =
{
	// Universal msg descriptors
	[ARCADIA_MSG_ID_NOOP] 							= "NOOP",

	// DRIVE msg descriptors
	[ARCADIA_MSG_ID_DRIVE_READ_NVM]					= "DRIVE_READ_NVM",
	[ARCADIA_MSG_ID_DRIVE_WRITE_NVM]				= "DRIVE_WRITE_NVM",
	[ARCADIA_MSG_ID_DRIVE_ERASE_NVM]				= "DRIVE_ERASE_NVM",

	// CHRONO msg descriptors
	[ARCADIA_MSG_ID_CHRONO_TIMER_ELAPSED]			= "CHRONO_TIMER_ELAPSED",
	[ARCADIA_MSG_ID_CHRONO_SCHEDULE_MSG_FOR_TASK]	= "CHRONO_SCHEDULE_MSG_FOR_TASK",
	[ARCADIA_MSG_ID_CHRONO_CANCEL_SCHEDULED_MSG]	= "CHRONO_CANCEL_SCHEDULED_MSG"
};

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void 	ARCADIA_create_task		(ARCADIA_task_id_t task_id);

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	Sets up a task by providing with it stack, message queue, and control block memory.
 *	Populates the given task's entry in the `rtos_tasks` table. Also performs a given task's 
 *	application-specific initialization, if applicable.
 *
 * 	@param[in] task_id The ID of the task to be created
 *
 ****************************************************************************************************/
static void ARCADIA_create_task(ARCADIA_task_id_t task_id)
{
	ARCADIA_rtos_task_t * p_rtos_task = &rtos_tasks[task_id];

	p_rtos_task->init();

	p_rtos_task->handle = xTaskCreateStatic(
		p_rtos_task->task,
		p_rtos_task->kpc_name,
		p_rtos_task->stack.u32_words,
		NULL,
		configMAX_PRIORITIES - 1U,
		p_rtos_task->stack.p_stack,
		&p_rtos_task->task_control_block
	);

	p_rtos_task->queue_handle = xQueueCreateStatic(
		ARCADIA_Q_LENGTH,
		ARCADIA_Q_ITEM_SIZE,
		p_rtos_task->u8_queue_storage,
		&p_rtos_task->queue
	);
}

/****************************************************************************************************
 *	Sets up all application tasks and starts the FreeRTOS scheduler
 *
 ****************************************************************************************************/
void NORETURN ARCADIA_start(void)
{
	ARCADIA_create_task(ARCADIA_TASK_ID_SHELL);
	ARCADIA_create_task(ARCADIA_TASK_ID_DRIVE);
	ARCADIA_create_task(ARCADIA_TASK_ID_CHRONO);

	vTaskStartScheduler();

	// Not reached
	while(1);
}

void vApplicationIdleHook(void)
{
    while(1);
}

TaskHandle_t ARCADIA_handle_from_id(ARCADIA_task_id_t task_id)
{
	ASSERT(task_id < ARCADIA_TASK_ID_NUM_IDS);

	TaskHandle_t handle = NULL;

	for (uint8_t i = 0; i < ARCADIA_TASK_ID_NUM_IDS; i++)
	{
		if (rtos_tasks[task_id].task_id == task_id)
		{
			handle = rtos_tasks[task_id].handle;
		}
	}

	return handle;
}

ARCADIA_task_id_t ARCADIA_get_current_task_id(void)
{
	TaskHandle_t current_handle = xTaskGetCurrentTaskHandle();

	ASSERT(current_handle != NULL);

	ARCADIA_task_id_t task_id = ARCADIA_TASK_ID_NUM_IDS;

	for (uint8_t i = 0; i < ARCADIA_TASK_ID_NUM_IDS; i++)
	{
		if (rtos_tasks[i].handle == current_handle)
		{
			task_id = rtos_tasks[i].task_id;
			break;
		}
	}

	return task_id;
}

uint32_t ARCADIA_send(ARCADIA_task_id_t task_id, ARCADIA_msg_t * p_msg)
{
	ASSERT(task_id < ARCADIA_TASK_ID_NUM_IDS);
	ASSERT(p_msg);

	// Notify SHELL task to unblock it from `ulTaskNotifyTake` (it's waiting on UART input or notifications).
	// This ensures it processes inbound messages immediately.
	if (ARCADIA_TASK_ID_SHELL == task_id)
	{
		xTaskNotifyGive(rtos_tasks[ARCADIA_TASK_ID_SHELL].handle);
	}

	return (uint32_t)xQueueSend(rtos_tasks[task_id].queue_handle, (const void *)p_msg, portMAX_DELAY);
}

uint32_t ARCADIA_send_from_isr(ARCADIA_task_id_t task_id, ARCADIA_msg_t * p_msg)
{
	ASSERT(task_id < ARCADIA_TASK_ID_NUM_IDS);
	ASSERT(p_msg);
	return (uint32_t)xQueueSendFromISR(rtos_tasks[task_id].queue_handle, (const void *)p_msg, NULL);
}

uint32_t ARCADIA_receive(ARCADIA_msg_t * p_msg)
{
	ASSERT(p_msg);
	return (uint32_t)xQueueReceive(rtos_tasks[ARCADIA_get_current_task_id()].queue_handle, (void * const)p_msg, portMAX_DELAY);
}

uint32_t ARCADIA_receive_nb(ARCADIA_msg_t * p_msg)
{
	ASSERT(p_msg);
	return (uint32_t)xQueueReceive(rtos_tasks[ARCADIA_get_current_task_id()].queue_handle, (void * const)p_msg, 0);
}

uint32_t ARCADIA_get_task_stack_size_words(ARCADIA_task_id_t task_id)
{
	ASSERT(task_id < ARCADIA_TASK_ID_NUM_IDS);
	return rtos_tasks[task_id].stack.u32_words;
}

const char * ARCADIA_get_task_name(ARCADIA_task_id_t task_id)
{
	ASSERT(task_id < ARCADIA_TASK_ID_NUM_IDS);
	return rtos_tasks[task_id].kpc_name;
}

const char * ARCADIA_get_msg_type(ARCADIA_msg_id_t msg_id)
{
	ASSERT(msg_id < ARCADIA_MSG_ID_NUM_IDS);
	return kpc_msg_descriptors[msg_id];
}
