#include "dac.h"
#include "io.h"

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void DAC_init(void)
{
	MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_DAC(1);

	DAC_REGS->DAC_CTRLB = DAC_CTRLB_EOEN(1);

	IO_enable_peripheral_function_for_pin(IO_PIN_ID_PA02, IO_PERIPHERAL_FUNCTION_B);

	DAC_REGS->DAC_CTRLA = DAC_CTRLA_ENABLE(1);

	while (DAC_REGS->DAC_SYNCBUSY & DAC_SYNCBUSY_ENABLE(1))
	{
		continue;
	}
}

void DAC_write(uint16_t u16_data)
{
	DAC_REGS->DAC_DATABUF =  u16_data;

	while (DAC_REGS->DAC_SYNCBUSY & DAC_SYNCBUSY_DATA(1))
	{
		continue;
	}
}
