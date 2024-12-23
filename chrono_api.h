#ifndef CHRONO_API_H
#define CHRONO_API_H

#include "common.h"
#include "arcadia.h"
#include "timer.h"

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

TIMER_id_t 			CHRONO_API_schedule_msg_for_task		(ARCADIA_msg_t * p_msg, ARCADIA_task_id_t task_id, uint16_t u16_delta_seconds, TIMER_mode_t mode);
ARCADIA_status_t 	CHRONO_API_cancel_scheduled_message		(TIMER_id_t timer_id);

uint8_t 			CHRONO_API_shell_sn						(uint8_t argc, char ** argv);
uint8_t 			CHRONO_API_shell_cancel					(uint8_t argc, char ** argv);

#endif // CHRONO_API_H
