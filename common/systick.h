#ifndef SYSTICK_H
#define SYSTICK_H

#include "samd21e18a.h"
#include "core_cm0plus.h"


void 		SYSTICK_init		(void);
uint32_t 	SYSTICK_get_ticks	(void);


#endif