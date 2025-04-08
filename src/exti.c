#include "exti.h"
#include "io.h"
#include "shell.h"

void EXTI_init(void)
{
	// Enable APB for EIC
	MCLK_REGS->MCLK_APBAMASK |= MCLK_APBAMASK_EIC(1);

	// Enable Generic Clock for EIC
    GCLK_REGS->GCLK_PCHCTRL[EIC_GCLK_ID] = GCLK_PCHCTRL_CHEN(1) | GCLK_PCHCTRL_GEN_GCLK0;

    while (!(GCLK_REGS->GCLK_PCHCTRL[EIC_GCLK_ID] & GCLK_PCHCTRL_CHEN(1)))
	{
		continue;
	}

	// temp, sense0, rising edge
	EIC_REGS->EIC_CONFIG[0] |= EIC_CONFIG_SENSE0_RISE;

	IO_enable_peripheral_function_for_pin(IO_PIN_ID_PA00, IO_PERIPHERAL_FUNCTION_A);

	// Enable
	EIC_REGS->EIC_CTRLA = EIC_CTRLA_ENABLE(1);

	// Stabilize
	while ((EIC_REGS->EIC_SYNCBUSY & EIC_SYNCBUSY_ENABLE(1)) == 0)
	{
		continue;
	}

	NVIC_EnableIRQ(EIC_IRQn);
}

void irqEIC(void)
{
	SHELL_printf("Wowie!\n");

	NVIC_ClearPendingIRQ(EIC_IRQn);
}