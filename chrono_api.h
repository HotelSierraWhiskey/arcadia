#ifndef CHRONO_API_H
#define CHRONO_API_H

#include "common.h"
#include "arcadia.h"
#include "timer.h"

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

TIMER_id_t CHRONO_API_schedule_msg_for_task(ARCADIA_msg_t * p_msg, ARCADIA_task_id_t task_id, uint16_t u16_delta_seconds);

#endif // CHRONO_API_H