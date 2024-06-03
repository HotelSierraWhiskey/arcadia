#include "clock.h"

void CLOCK_osc8m_init(void)
{
	SYSCTRL_REGS->SYSCTRL_OSC8M |= SYSCTRL_OSC8M_PRESC(0) | SYSCTRL_OSC8M_ENABLE(1) | SYSCTRL_OSC8M_RUNSTDBY(1);
}

void CLOCK_gclock_init(const CLOCK_gclock_id gclock_id, const CLOCK_source_t source)
{
	GCLK_REGS->GCLK_GENCTRL = GCLK_CLKCTRL_ID(gclock_id) | GCLK_GENCTRL_SRC(source) | GCLK_GENCTRL_IDC(1) | GCLK_GENCTRL_GENEN(1);

	// Wait for register synchronization
	while (GCLK_REGS->GCLK_STATUS & GCLK_STATUS_SYNCBUSY(1))
	{
		continue;
	}
}
