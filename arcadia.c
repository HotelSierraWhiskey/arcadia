#include "arcadia.h"
#include "common.h"
#include "FreeRTOSConfig.h"
#include "shell.h"

static void task1( void * parameters )
{
    ( void ) parameters;

    for( ; ; )
    {
        vTaskDelay( 500 );
		SHELL_printf("PRASK1\r\n");
    }
}

static void task2( void * parameters )
{
    ( void ) parameters;
	
    for( ; ; )
    {
        vTaskDelay( 1000 );
		SHELL_printf("MEEPMEEP\r\n");
    }
}

static void task3( void * parameters )
{
    ( void ) parameters;
	
    for( ; ; )
    {
        vTaskDelay( 1000 );
		SHELL_printf("BLASTER!\r\n");
    }
}

typedef void (* ARCADIA_task_t)(void *);

typedef struct _ARCADIA_rtos_task
{
	ARCADIA_task_id			task_id;
	const char * const		kpc_name;
	ARCADIA_task_t			task;
	StaticTask_t			task_control_block;
	StackType_t				stack[configMINIMAL_STACK_SIZE];

	TaskHandle_t			task_handle;
} ARCADIA_rtos_task_t;

static ARCADIA_rtos_task_t rtos_tasks[ARCADIA_TASK_ID_NUM_IDS] =
{
	{
		.task_id 	= ARCADIA_TASK_ID_SHELL,
		.kpc_name 	= "SHELL",
		.task 		= SHELL_task,
		.task_handle = NULL
	},
	{
		.task_id 	= ARCADIA_TASK_ID_2,
		.kpc_name 	= "TASK1",
		.task 		= task2,
		.task_handle = NULL
	},
	{
		.task_id 	= ARCADIA_TASK_ID_3,
		.kpc_name 	= "TASK1",
		.task 		= task3,
		.task_handle = NULL
	}
};

static void ARCADIA_create_task(ARCADIA_task_id task_id)
{
	ARCADIA_rtos_task_t * p_rtos_task = &rtos_tasks[task_id];

	xTaskCreateStatic(
		p_rtos_task->task,
		p_rtos_task->kpc_name,
		configMINIMAL_STACK_SIZE,
		NULL,
		configMAX_PRIORITIES - 1U,
		p_rtos_task->stack,
		&p_rtos_task->task_control_block
	);
	// xTaskCreate(
	// p_rtos_task->task,
	// p_rtos_task->kpc_name,
	// configMINIMAL_STACK_SIZE,
	// NULL,
	// configMAX_PRIORITIES - 1U,
	// &p_rtos_task->task_handle
	// );
}

void ARCADIA_start(void)
{
	ARCADIA_create_task(ARCADIA_TASK_ID_SHELL);
	// ARCADIA_create_task(ARCADIA_TASK_ID_2);
	// ARCADIA_create_task(ARCADIA_TASK_ID_3);

    vTaskStartScheduler();
}
