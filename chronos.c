#include "chronos.h"
#include "io.h"
#include "sys.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define SYS_TICK_FREQ_1MS(source_clock_freq) ((source_clock_freq) / (1000U))

/****************************************************************************************************
 *	V A R I A B L E S
 ****************************************************************************************************/

static volatile uint32_t u32_ticks;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void CHRONOS_init(void)
{
	uint32_t u32_source_clock_freq = SYS_get_source_clock_freq();
	u32_ticks = 0;
	SysTick_Config(SYS_TICK_FREQ_1MS(u32_source_clock_freq));
}

uint32_t CHRONOS_get_ticks(void)
{
	return u32_ticks;
}

uint32_t CHRONOS_ticks_since(uint32_t start_ticks)
{
	uint32_t current_ticks = CHRONOS_get_ticks();
	return current_ticks - start_ticks;
}

void CHRONOS_delay_ms(uint32_t u32_ms)
{
	uint32_t start_ticks = CHRONOS_get_ticks();
	
	while (CHRONOS_ticks_since(start_ticks) < u32_ms)
	{
		continue;
	}
}

void irqSysTick()
{
	IO_set_pin(IO_PIN_ID_PA27, IO_PIN_STATE_HIGH);
	u32_ticks++;
	IO_set_pin(IO_PIN_ID_PA27, IO_PIN_STATE_LOW);
}
