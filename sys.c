#include "common.h"
#include "sys.h"
#include "io.h"

static void SYS_clock_init(void);

void SYS_init(void)
{
	SYS_clock_init();
}

static void SYS_clock_init(void)
{
	NVMCTRL_REGS->NVMCTRL_CTRLB = NVMCTRL_CTRLB_RWS(1) | NVMCTRL_CTRLB_MANW(1);

	while ((OSCCTRL_REGS->OSCCTRL_STATUS & OSCCTRL_STATUS_OSC48MRDY(1)) == 0)
	{
		continue;
	}

	OSCCTRL_REGS->OSCCTRL_OSC48MCTRL = OSCCTRL_OSC48MCTRL_ENABLE(1) | OSCCTRL_OSC48MCTRL_ONDEMAND(1);
	OSCCTRL_REGS->OSCCTRL_OSC48MDIV = OSCCTRL_OSC48MDIV_DIV_DIV2;
	OSCCTRL_REGS->OSCCTRL_OSC48MSTUP = OSCCTRL_OSC48MSTUP_STARTUP_CYCLE1024;

	while (OSCCTRL_REGS->OSCCTRL_OSC48MSYNCBUSY & OSCCTRL_OSC48MSYNCBUSY_OSC48MDIV(1))
	{
		continue;
	}


	while ((OSCCTRL_REGS->OSCCTRL_STATUS & OSCCTRL_STATUS_OSC48MRDY(1)) == 0)
	{
		continue;
	}

	// Provide GCLK0 with OSC48M as a clock source
	GCLK_REGS->GCLK_GENCTRL[0] = GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_OSC48M) | 
                                 GCLK_GENCTRL_GENEN(1) |
								 GCLK_GENCTRL_DIVSEL(0) |
								 GCLK_GENCTRL_DIV(0) |
								 GCLK_GENCTRL_IDC(1) |
                                 GCLK_GENCTRL_OE(1);

	// Set the PORT's APB
	MCLK_REGS->MCLK_APBBMASK |= MCLK_APBBMASK_PORT(1);

	// Output the clock signal on PA27
	IO_enable_peripheral_function_for_pin(IO_PIN_ID_PA27, IO_PERIPHERAL_FUNCTION_H);
}