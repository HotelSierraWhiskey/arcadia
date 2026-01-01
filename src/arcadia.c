#include "arcadia.h"
#include "common.h"
#include "FreeRTOSConfig.h"
#include "utils.h"
#include "shell.h"
#include "drive.h"
#include "chrono.h"
#include "load.h"
#include "button.h"

#ifdef JLINK_MUX
#include "target_port.h"
#endif // JLINK_MUX

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define ARCADIA_Q_LENGTH				(3U)
#define ARCADIA_Q_ITEM_SIZE				sizeof(ARCADIA_msg_t)

#define ARCADIA_SHELL_STACK_SIZE		BYTES_TO_WORDS(1536U)
#define ARCADIA_DRIVE_STACK_SIZE		BYTES_TO_WORDS(1536U)
#define ARCADIA_CHRONO_STACK_SIZE		BYTES_TO_WORDS(512U) 	// was 1k
#define ARCADIA_LOAD_STACK_SIZE			BYTES_TO_WORDS(1024U)

/**
 *	Main task loop function pointer prototype
 */
typedef void (* ARCADIA_task_t)(void *);

/**
 *	Pre-kernel task initialization function pointer prototype
 */
typedef void (* ARCADIA_task_init_t)(void);

/**
 * 	Task stack typedef
 */
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
 *	MEDIA stack
 */
StackType_t load_stack[ARCADIA_LOAD_STACK_SIZE];

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
		{
		.task_id 	= ARCADIA_TASK_ID_LOAD,
		.kpc_name 	= "LOAD",
		.stack 		= 
		{
			.p_stack 	= load_stack,
			.u32_words 	= ARCADIA_LOAD_STACK_SIZE
		},
		.task 		= LOAD_task,
		.init		= LOAD_init
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
	[ARCADIA_MSG_ID_DRIVE_WRITE_BOOT_ROW]			= "DRIVE_WRITE_BOOT_ROW",
	[ARCADIA_MSG_ID_DRIVE_OPEN_FILE]				= "DRIVE_OPEN_FILE",
	[ARCADIA_MSG_ID_DRIVE_CLOSE_FILE]				= "DRIVE_CLOSE_FILE",
	[ARCADIA_MSG_ID_DRIVE_FETCH_FNAMES]				= "DRIVE_FETCH_FNAMES",
	[ARCADIA_MSG_ID_DRIVE_CHDIR]					= "DRIVE_CHDIR",
	[ARCADIA_MSG_ID_DRIVE_WRITE]					= "DRIVE_WRITE",
	[ARCADIA_MSG_ID_DRIVE_READ]						= "DRIVE_READ",
	[ARCADIA_MSG_ID_DRIVE_SEEK]						= "DRIVE_SEEK",

	// CHRONO msg descriptors
	[ARCADIA_MSG_ID_CHRONO_TIMER_ELAPSED]			= "CHRONO_TIMER_ELAPSED",
	[ARCADIA_MSG_ID_CHRONO_SCHEDULE_MSG_FOR_TASK]	= "CHRONO_SCHEDULE_MSG_FOR_TASK",
	[ARCADIA_MSG_ID_CHRONO_CANCEL_SCHEDULED_MSG]	= "CHRONO_CANCEL_SCHEDULED_MSG",
	[ARCADIA_MSG_ID_CHRONO_DEBOUNCE_EXTI]			= "CHRONO_DEBOUNCE_EXTI",
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
	ARCADIA_create_task(ARCADIA_TASK_ID_LOAD);

	vTaskStartScheduler();

	// Not reached
	while(1);
}

/****************************************************************************************************
 *  Idle task hook function
 * 
 *  This function is called when the system is idle.
 * 
 ****************************************************************************************************/
void vApplicationIdleHook(void)
{
#ifdef JLINK_MUX
	TARGET_PORT_deselect_all();
	BUTTON_register_callback(BUTTON_ID_PORT_0, TARGET_PORT_select_0);
	BUTTON_register_callback(BUTTON_ID_PORT_1, TARGET_PORT_select_1);
	BUTTON_register_callback(BUTTON_ID_PORT_2, TARGET_PORT_select_2);
	BUTTON_register_callback(BUTTON_ID_PORT_3, TARGET_PORT_select_3);
	BUTTON_register_callback(BUTTON_ID_PORT_4, TARGET_PORT_select_4);
	BUTTON_register_callback(BUTTON_ID_PORT_5, TARGET_PORT_select_5);
	BUTTON_register_callback(BUTTON_ID_PORT_6, TARGET_PORT_select_6);
	BUTTON_register_callback(BUTTON_ID_PORT_7, TARGET_PORT_select_7);
#endif // JLINK_MUX

    while(1)
	{
		BUTTON_update();
	}
}

/****************************************************************************************************
 * 	Retrieves the task handle associated with a given task ID
 *
 * 	@param[in] task_id The ID of the task whose handle is requested.
 *
 * 	@return TaskHandle_t Handle to the task if found, otherwise NULL.
 ****************************************************************************************************/
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

/****************************************************************************************************
 *  Retrieves the task ID of the currently executing task
 *
 *  @return Task ID of the calling task
 ****************************************************************************************************/
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

	// Something is very wrong if this trips
	ASSERT(task_id != ARCADIA_TASK_ID_NUM_IDS);

	return task_id;
}

/****************************************************************************************************
 *  Sends a message to the specified task's message queue
 *  
 *  Ensures that the target task ID is within bounds and that the message pointer is valid.
 *  If the target task is the SHELL task, it also notifies the task to wake it up from
 *  `ulTaskNotifyTake`, ensuring it processes incoming messages immediately.
 *
 *  @param[in] task_id The ID of the task to send the message to.
 *  @param[in] p_msg Pointer to the message to be sent.
 *
 *  @return `pdPASS` if the message was successfully added to the queue.
 ****************************************************************************************************/
uint32_t ARCADIA_send(ARCADIA_task_id_t task_id, ARCADIA_msg_t * p_msg)
{
	ASSERT(task_id < ARCADIA_TASK_ID_NUM_IDS);
	ASSERT(p_msg);

	// Notify SHELL to unblock from `ulTaskNotifyTake` (it's waiting on UART input or notifications).
	// This ensures SHELL processes inbound messages immediately.
	if (ARCADIA_TASK_ID_SHELL == task_id)
	{
		xTaskNotifyGive(rtos_tasks[ARCADIA_TASK_ID_SHELL].handle);
	}

	return (uint32_t)xQueueSend(rtos_tasks[task_id].queue_handle, (const void *)p_msg, portMAX_DELAY);
}

/****************************************************************************************************
 *  Sends a message to the specified task's queue from ISR context
 *
 *	@param[in] task_id The ID of the task to send the message to.
 *	@param[in] p_msg Pointer to the message to be sent.
 *
 *  @return `pdPASS` if the message was successfully added to the queue.
 ****************************************************************************************************/
uint32_t ARCADIA_send_from_isr(ARCADIA_task_id_t task_id, ARCADIA_msg_t * p_msg)
{
	ASSERT(task_id < ARCADIA_TASK_ID_NUM_IDS);
	ASSERT(p_msg);
	return (uint32_t)xQueueSendFromISR(rtos_tasks[task_id].queue_handle, (const void *)p_msg, NULL);
}

/****************************************************************************************************
 *	Receives a message from the current task's queue, blocking until a message is available.
 *  
 *	This function retrieves the message from the queue of the currently executing task.
 *
 *	@param[out] p_msg pointer to the message structure where the received data will be stored.
 *
 *	@return	`pdPASS` if a message was successfully received.
 ****************************************************************************************************/
uint32_t ARCADIA_receive(ARCADIA_msg_t * p_msg)
{
	ASSERT(p_msg);
	return (uint32_t)xQueueReceive(rtos_tasks[ARCADIA_get_current_task_id()].queue_handle, (void * const)p_msg, portMAX_DELAY);
}

/****************************************************************************************************
 *	Receives a message from the current task's queue without blocking.
 *  
 *	This function attempts to retrieve a message from the queue of the currently executing task.
 *	If no message is available, it returns immediately.
 *
 *	@param[out] p_msg  Pointer to the message structure where the received data will be stored.
 *
 *	@return `pdPASS` if a message was successfully received, `errQUEUE_EMPTY` otherwise.
 ****************************************************************************************************/
uint32_t ARCADIA_receive_nb(ARCADIA_msg_t * p_msg)
{
	ASSERT(p_msg);
	return (uint32_t)xQueueReceive(rtos_tasks[ARCADIA_get_current_task_id()].queue_handle, (void * const)p_msg, 0);
}

/****************************************************************************************************
 *	Retrieves a task's stack size (in words)
 *
 *	@param[in] task_id The ID of the task whose stack size is requested.
 *
 *	@return The stack size of the specified task in words.
 ****************************************************************************************************/
uint32_t ARCADIA_get_task_stack_size_words(ARCADIA_task_id_t task_id)
{
	ASSERT(task_id < ARCADIA_TASK_ID_NUM_IDS);
	return rtos_tasks[task_id].stack.u32_words;
}

/****************************************************************************************************
 *  Retrieves the name of a specified task.
 *
 *  @param[in] task_id The ID of the task whose name is requested.
 *
 *  @return pointer to the name of the specified task.
 ****************************************************************************************************/
const char * ARCADIA_get_task_name(ARCADIA_task_id_t task_id)
{
	ASSERT(task_id < ARCADIA_TASK_ID_NUM_IDS);
	return rtos_tasks[task_id].kpc_name;
}

/****************************************************************************************************
 *  Retrieves the string representation of a message type.
 *
 *	@param[in] msg_id The ID of the message whose type is requested.
 *
 *  @return pointer to the message type descriptor string.
 ****************************************************************************************************/
const char * ARCADIA_get_msg_type(ARCADIA_msg_id_t msg_id)
{
	ASSERT(msg_id < ARCADIA_MSG_ID_NUM_IDS);
	return kpc_msg_descriptors[msg_id];
}
