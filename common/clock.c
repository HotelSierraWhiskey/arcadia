#include "clock.h"


void CLOCK_osc8m_init(void)
{
	SYSCTRL_REGS->SYSCTRL_OSC8M |= SYSCTRL_OSC8M_PRESC(SYSCTRL_OSC8M_PRESC_0);// | SYSCTRL_OSC8M_ENABLE(1);// | SYSCTRL_OSC8M_RUNSTDBY(1)
}

void CLOCK_gclock_init(const CLOCK_gclock_id gclock_id, const CLOCK_source_t source)
{
	GCLK_REGS->GCLK_GENCTRL = GCLK_CLKCTRL_ID(GCLK_CLKCTRL_GEN_GCLK0) | GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_OSC8M) | GCLK_GENCTRL_IDC(1) | GCLK_GENCTRL_OE(1) | GCLK_GENCTRL_GENEN(1) | GCLK_GENCTRL_OOV(1);

	// Wait for register synchronization
	while (GCLK_REGS->GCLK_STATUS & GCLK_STATUS_SYNCBUSY(1))
	{
		continue;
	}
}

void CLOCK_main_clock_output_enable(const IO_pin_designation_t pin)
{
	GCLK_REGS->GCLK_GENCTRL |= GCLK_GENCTRL_OE(1);
	IO_pin_pmux_enable(pin, IO_PERIPHERAL_FUNCTION_H);
}

void CLOCK_main_clock_output_disable(const IO_pin_designation_t pin)
{
	GCLK_REGS->GCLK_GENCTRL |= GCLK_GENCTRL_OE(0);
	IO_pin_pmux_disable(pin, IO_PERIPHERAL_FUNCTION_H);
}