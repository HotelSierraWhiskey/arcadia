#include "dac.h"

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void DAC_init(void)
{
	MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_DAC(1);

	DAC_REGS->DAC_CTRLB = DAC_CTRLB_EOEN(1);

	DAC_REGS->DAC_CTRLA = DAC_CTRLA_ENABLE(1);

	while (DAC_REGS->DAC_SYNCBUSY & DAC_SYNCBUSY_ENABLE(1))
	{
		continue;
	}
}
