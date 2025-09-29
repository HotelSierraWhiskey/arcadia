#ifndef EXTI_H
#define EXTI_H

#include "common.h"
#include "io.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

typedef enum _EXTI_source_id
{
	EXTI_SOURCE_ID_DEBUG_BUTTON = 0,
	//////////
	EXTI_SOURCE_ID_NUM_IDS
} EXTI_source_id_t;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void 			EXTI_init					(void);
IO_pin_id_t		EXTI_get_pin_from_source	(EXTI_source_id_t source);
bool			EXTI_source_asserted		(EXTI_source_id_t source);
void			EXTI_deassert_source		(EXTI_source_id_t source);
void 			EXTI_enable_isr				(EXTI_source_id_t source);
void 			EXTI_disable_isr			(EXTI_source_id_t source);

#endif // EXTI_H
