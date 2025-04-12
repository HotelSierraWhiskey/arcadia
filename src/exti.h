#ifndef EXTI_H
#define EXTI_H

#include "common.h"

typedef enum _EXTI_source_id
{
	EXTI_SOURCE_ID_DEBUG_BUTTON = 0,
	//////////
	EXTI_SOURCE_ID_NUM_IDS
} EXTI_source_id_t;

void 	EXTI_init	(void);

#endif // EXTI_H
