#include "arcadia.h"
#include "common.h"
#include "FreeRTOSConfig.h"
#include "shell.h"
#include "drive.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define ARCADIA_Q_LENGTH	(5)
#define ARCADIA_Q_ITEM_SIZE	sizeof(uint32_t)

typedef void (* ARCADIA_task_t)(void *);

typedef struct _ARCADIA_rtos_task
{
	ARCADIA_task_id			task_id;
	const char * const		kpc_name;
	ARCADIA_task_t			task;
	StaticTask_t			task_control_block;
	StackType_t				stack[configMINIMAL_STACK_SIZE];
	TaskHandle_t			handle;

	StaticQueue_t			queue;
	uint8_t					u8_queue_storage[ARCADIA_Q_LENGTH * ARCADIA_Q_ITEM_SIZE];
	QueueHandle_t			queue_handle;
} ARCADIA_rtos_task_t;

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

static ARCADIA_rtos_task_t rtos_tasks[ARCADIA_TASK_ID_NUM_IDS] =
{
	{
		.task_id 	= ARCADIA_TASK_ID_SHELL,
		.kpc_name 	= "SHELL",
		.task 		= SHELL_task,
	},
	{
		.task_id 	= ARCADIA_TASK_ID_DRIVE,
		.kpc_name 	= "DRIVE",
		.task 		= DRIVE_task,
	},
};



/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

static void ARCADIA_create_task(ARCADIA_task_id task_id)
{
	ARCADIA_rtos_task_t * p_rtos_task = &rtos_tasks[task_id];

	p_rtos_task->handle = xTaskCreateStatic(
		p_rtos_task->task,
		p_rtos_task->kpc_name,
		configMINIMAL_STACK_SIZE,
		NULL,
		configMAX_PRIORITIES - 1U,
		p_rtos_task->stack,
		&p_rtos_task->task_control_block
	);

	p_rtos_task->queue_handle = xQueueCreateStatic(
		ARCADIA_Q_LENGTH,
		ARCADIA_Q_ITEM_SIZE,
		p_rtos_task->u8_queue_storage,
		&p_rtos_task->queue
	);
}

void ARCADIA_start(void)
{
	ARCADIA_create_task(ARCADIA_TASK_ID_SHELL);
	ARCADIA_create_task(ARCADIA_TASK_ID_DRIVE);

    vTaskStartScheduler();
}

TaskHandle_t ARCADIA_handle_from_id(ARCADIA_task_id task_id)
{
	// assert task_id
	TaskHandle_t handle;

	for (uint8_t i = 0; i < ARCADIA_TASK_ID_NUM_IDS; i++)
	{
		if (rtos_tasks[task_id].task_id == task_id)
		{
			handle = rtos_tasks[task_id].handle;
		}
	}

	return handle;
}

uint32_t ARCADIA_send(ARCADIA_task_id task_id, const void * kp_item, TickType_t ticks_to_wait)
{
	return (uint32_t)xQueueSend(rtos_tasks[task_id].queue_handle, kp_item, ticks_to_wait);
}

uint32_t ARCADIA_receive(ARCADIA_task_id task_id, void * p_buffer, TickType_t ticks_to_wait)
{
	return (uint32_t)xQueueReceive(rtos_tasks[task_id].queue_handle, p_buffer, ticks_to_wait);
}
