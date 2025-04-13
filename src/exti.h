#ifndef EXTI_H
#define EXTI_H

#include "common.h"
#include "io.h"

typedef enum _EXTI_source_type
{
	EXTI_SOURCE_TYPE_BUTTON,
	//////////
	EXTI_SOURCE_TYPE_NUM_TYPES
} EXTI_source_type_t;

typedef enum _EXTI_source_id
{
	EXTI_SOURCE_ID_DEBUG_BUTTON = 0,
	//////////
	EXTI_SOURCE_ID_NUM_IDS
} EXTI_source_id_t;

void 			EXTI_init					(void);
IO_pin_id_t		EXTI_get_pin_from_source	(EXTI_source_id_t source);
bool			EXTI_source_asserted		(EXTI_source_id_t source);
void			EXTI_deassert_source		(EXTI_source_id_t source);

#endif // EXTI_H
