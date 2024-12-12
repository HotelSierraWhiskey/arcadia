#include "sys.h"
#include "io.h"
#include "uart.h"
#include "shell.h"
#include "nvmctrl.h"

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

/* Standard includes. */
#include <stdio.h>

#define xPortSysTickHandler irqSysTick

void vApplicationMallocFailedHook( void ) {
  SHELL_printf("MALLOC FAILED !!\n");
  configASSERT(0);
}

/*-----------------------------------------------------------*/

static void exampleTask( void * parameters );

/*-----------------------------------------------------------*/

static void exampleTask( void * parameters )
{
    /* Unused parameters. */
    ( void ) parameters;

	// SHELL_printf("sup\r\n");

    for( ; ; )
    {
        /* Example Task Code */
        // vTaskDelay( 1000 ); /* delay 100 ticks */
    }
}
/*-----------------------------------------------------------*/

void main( void )
{
	SYS_init();
	SHELL_init();

    static StaticTask_t exampleTaskTCB;
    static StackType_t exampleTaskStack[ configMINIMAL_STACK_SIZE ];

    SHELL_printf( "Check 1\r\n" );

    ( void ) xTaskCreateStatic( exampleTask,
                                "example",
                                configMINIMAL_STACK_SIZE,
                                NULL,
                                configMAX_PRIORITIES - 1U,
                                &( exampleTaskStack[ 0 ] ),
                                &( exampleTaskTCB ) );

    SHELL_printf( "Check 2\r\n" );

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
