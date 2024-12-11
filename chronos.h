#ifndef SYS_TIME_H
#define SYS_TIME_H

#include "common.h"

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void 		CHRONOS_init 			(void);
uint32_t 	CHRONOS_get_ticks 		(void);
uint32_t 	CHRONOS_ticks_since		(uint32_t start_ticks);
void 		CHRONOS_delay_ms		(uint32_t u32_ms);

#endif // SYS_TIME_H
