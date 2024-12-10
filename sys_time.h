#ifndef SYS_TIME_H
#define SYS_TIME_H

#include "common.h"

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void 		SYS_TIME_init 			(void);
uint32_t 	SYS_TIME_get_ticks 		(void);
uint32_t 	SYS_TIME_ticks_since	(uint32_t start_ticks);
void 		SYS_TIME_delay_ms		(uint32_t u32_ms);

#endif // SYS_TIME_H
