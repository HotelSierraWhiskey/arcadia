#include "chronos.h"
#include "io.h"
#include "sys.h"
#include <FreeRTOS.h>


/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define SYS_TICK_FREQ_1MS(source_clock_freq) ((source_clock_freq) / (1000U))

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void		CHRONOS_init	(void);

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void vPortSetupTimerInterrupt(void)
{
	CHRONOS_init();
}

static void CHRONOS_init(void)
{
	uint32_t u32_source_clock_freq = SYS_get_source_clock_freq();
	SysTick_Config(SYS_TICK_FREQ_1MS(u32_source_clock_freq));
}

uint32_t CHRONOS_ticks_since(uint32_t start_ticks)
{
	uint32_t current_ticks = CHRONOS_get_ticks();
	return current_ticks - start_ticks;
}
