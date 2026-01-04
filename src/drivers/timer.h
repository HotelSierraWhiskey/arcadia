#ifndef TIMER_H
#define TIMER_H

#include "common.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define TIMER_INVALID						(-1)
#define TIMER_AVAILABLE						(0U)
#define TIMER_REPEAT						(true)
#define TIMER_SINGLE_SHOT					(false)
#define TIMER_PRESCALED_SECOND_COUNT_VALUE	(32U)

/**
 *	Timer IDs
 */
typedef enum _TIMER_id
{
	TIMER_ID_0 = 0,
	TIMER_ID_1,
	TIMER_ID_2,
	TIMER_ID_3,
	//////////
	TIMER_ID_NUM_TIMERS
} TIMER_id_t;

/**
 *	Timer operation modes
 */
typedef enum _TIMER_mode
{
	TIMER_MODE_SINGLE_SHOT = 0,
	TIMER_MODE_REPEAT,
	//////////
	TIMER_MODE_NUM_MODES
} TIMER_mode_t;

/**
 *	Logical timer channel information
 */
typedef struct _TIMER_info
{
	uint64_t			u64_period_ms;
	TIMER_mode_t		mode;
	tc_registers_t *	p_timer_regs;
	uint8_t				u8_irq_id;
} TIMER_info_t;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void 					TIMER_init					(void);
void 					TIMER_start					(const TIMER_id_t k_timer_id);
void 					TIMER_stop					(const TIMER_id_t k_timer_id);
TIMER_id_t				TIMER_alloc					(uint64_t u64_period_ms, TIMER_mode_t mode);
const TIMER_info_t *	TIMER_get_timer_info		(const TIMER_id_t k_timer_id);
uint16_t		 		TIMER_get_timer_count		(const TIMER_id_t k_timer_id);
void 					TIMER_start_dma_timer		(void);
void 					TIMER_stop_dma_timer		(void);

/****************************************************************************************************
 *	S H E L L   F U N C T I O N S
 ****************************************************************************************************/

uint8_t					TIMER_shell_start_timer		(uint8_t argc, char ** argv);
uint8_t					TIMER_shell_stop_timer		(uint8_t argc, char ** argv);
uint8_t					TIMER_shell_info			(uint8_t argc, char ** argv);

#endif // TIMER_H
