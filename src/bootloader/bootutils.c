#include "bootutils.h"


#define BOOTUTILS_SYS_CLOCK_FREQ	(48000000U)


void BOOTUTILS_delay_ms (uint32_t u32_ms)
{
	SysTick->LOAD = (BOOTUTILS_SYS_CLOCK_FREQ / 1000U) - 1U;
	SysTick->VAL  = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk; // NO TICKINT

	while (u32_ms--)
	{
		// wait for wrap
		while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0)
		{
			__asm__ volatile("nop");
		}
	}

	SysTick->CTRL = 0;
}
