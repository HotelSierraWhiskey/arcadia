#include "sys.h"
#include "io.h"
#include "uart.h"
#include "shell.h"
#include "nvmctrl.h"
#include "chronos.h"

/****************************************************************************************************
 *	M A I N
 ****************************************************************************************************/
#include "FreeRTOSConfig.h"

/* FreeRTOS includes. */
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>

// /* Standard includes. */
// #include <stdio.h>

#define xPortSysTickHandler irqSysTick

void vApplicationMallocFailedHook( void ) {
  SHELL_printf("MALLOC FAILED !!\n");
  configASSERT(0);
}

/*-----------------------------------------------------------*/

static void task1( void * parameters );

/*-----------------------------------------------------------*/

static void task1( void * parameters )
{
    ( void ) parameters;

    for( ; ; )
    {
        vTaskDelay( 1000 );
		SHELL_printf("Ping\r\n");
    }
}

static void task2( void * parameters );

/*-----------------------------------------------------------*/

static void task2( void * parameters )
{
    ( void ) parameters;
	
    for( ; ; )
    {
        vTaskDelay( 1000 );
		SHELL_printf("Pong\r\n");
    }
}

/*-----------------------------------------------------------*/


static void task3( void * parameters );

/*-----------------------------------------------------------*/

static void task3( void * parameters )
{
    ( void ) parameters;
	
    for( ; ; )
    {
        vTaskDelay( 1000 );
		SHELL_printf("Pang\r\n");
    }
}

/*-----------------------------------------------------------*/

void main( void )
{
	SYS_init();
	SHELL_init();

    static StaticTask_t task1TCB;
    static StackType_t task1Stack[ configMINIMAL_STACK_SIZE ];

	static StaticTask_t task2TCB;
    static StackType_t task2Stack[ configMINIMAL_STACK_SIZE ];

	static StaticTask_t task3TCB;
    static StackType_t task3Stack[ configMINIMAL_STACK_SIZE ];

    ( void ) xTaskCreateStatic( task1,
                                "task1",
                                configMINIMAL_STACK_SIZE,
                                NULL,
                                configMAX_PRIORITIES - 1U,
                                &( task1Stack[ 0 ] ),
                                &( task1TCB ) );

	( void ) xTaskCreateStatic( task2,
							"task2",
							configMINIMAL_STACK_SIZE,
							NULL,
							configMAX_PRIORITIES - 1U,
							&( task2Stack[ 0 ] ),
							&( task2TCB ) );

	( void ) xTaskCreateStatic( task3,
						"task3",
						configMINIMAL_STACK_SIZE,
						NULL,
						configMAX_PRIORITIES - 1U,
						&( task3Stack[ 0 ] ),
						&( task3TCB ) );

    /* Start the scheduler. */
    vTaskStartScheduler();

    for( ; ; )
    {
        /* Should not reach here. */
    }
}
/*-----------------------------------------------------------*/

#if ( configCHECK_FOR_STACK_OVERFLOW > 0 )

    void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                        char * pcTaskName )
    {
        /* Check pcTaskName for the name of the offending task,
         * or pxCurrentTCB if pcTaskName has itself been corrupted. */
        ( void ) xTask;
        ( void ) pcTaskName;
    }

#endif /* #if ( configCHECK_FOR_STACK_OVERFLOW > 0 ) */
/*-----------------------------------------------------------*/


// int main(int argc, char ** argv) 
// {
// 	// SYS_init();

// 	// NVMCTRL_init();

// 	// SHELL_init();

// 	// IO_config_pin_direction(IO_PIN_ID_PA27, IO_DIRECTION_OUTPUT);

// 	// while (1)
// 	// {
// 	// 	SHELL_run();
// 	// }


// }
