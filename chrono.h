#ifndef SYS_TIME_H
#define SYS_TIME_H

#include "common.h"
#include "timer.h"
#include "arcadia.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define CHRONO_delay_ms(ms)				vTaskDelay(ms)
#define CHRONO_get_ticks()				xTaskGetTickCount()

typedef struct _CHRONO_msg_schedule_entry
{
	ARCADIA_task_id_t	task_id;
	ARCADIA_msg_t		msg;
} CHRONO_msg_schedule_entry_t;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void							CHRONO_init						(void);
void							CHRONO_task						(void * p_params);
CHRONO_msg_schedule_entry_t *	CHRONO_get_msg_schedule_entry	(TIMER_id_t timer_id);
uint32_t 						CHRONO_ticks_since				(uint32_t start_ticks);

#endif // CHRONO_H
