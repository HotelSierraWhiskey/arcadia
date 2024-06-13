#include "systick.h"
#include "io.h"


// Theoretically, 8MHz main clock frequency divided by 1000 ticks (desired, per second) minus one
// Calibrated manually against actual main clock frequency
#define SYSTICK_RELOAD (8029)

static uint32_t u32_ticks = 0;

void SYSTICK_init(void)
{
	SysTick_Config(SYSTICK_RELOAD);
	NVIC_EnableIRQ(SysTick_IRQn);
}

void irqSysTick(void)
{
	u32_ticks++;
	// IO_pin_toggle(IO_PIN_PA06);
}

uint32_t SYSTICK_get_ticks(void)
{
	return u32_ticks;
}