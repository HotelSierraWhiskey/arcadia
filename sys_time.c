#include "sys_time.h"
#include "io.h"
#include "sys.h"

#define SYS_TICK_FREQ_1MS(source_clock_freq) ((source_clock_freq) / (1000U))

static volatile uint32_t u32_ticks;

void SYS_TIME_init(void)
{
	uint32_t u32_source_clock_freq = SYS_get_source_clock_freq();
	u32_ticks = 0;
	SysTick_Config(SYS_TICK_FREQ_1MS(u32_source_clock_freq));
}

uint32_t SYS_TIME_get_ticks(void)
{
	return u32_ticks;
}

void irqSysTick()
{
	IO_set_pin(IO_PIN_ID_PA27, IO_PIN_STATE_HIGH);
	u32_ticks++;
	IO_set_pin(IO_PIN_ID_PA27, IO_PIN_STATE_LOW);
}
