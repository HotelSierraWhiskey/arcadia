#include "dac.h"
#include "io.h"
#include "shell.h"
#include "chrono.h"
#include "utils.h"

#define DAC_MAXVAL (0x03FFU)

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	Initializes the DAC peripheral
 *
 ****************************************************************************************************/
void DAC_init(void)
{
	// Enable APB for DAC
	MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_DAC(1);

	// Enable Generic Clock for DAC
    GCLK_REGS->GCLK_PCHCTRL[DAC_GCLK_ID] = GCLK_PCHCTRL_CHEN(1) | GCLK_PCHCTRL_GEN_GCLK0;

    while (!(GCLK_REGS->GCLK_PCHCTRL[DAC_GCLK_ID] & GCLK_PCHCTRL_CHEN(1)))
	{
		continue;
	}

	IO_enable_peripheral_function_for_pin(IO_PIN_ID_PA02, IO_PERIPHERAL_FUNCTION_B);

	DAC_REGS->DAC_CTRLB = DAC_CTRLB_EOEN(1);

	DAC_REGS->DAC_CTRLA = DAC_CTRLA_ENABLE(1);

	while (DAC_REGS->DAC_SYNCBUSY & DAC_SYNCBUSY_ENABLE(1))
	{
		continue;
	}
}

/****************************************************************************************************
 *	Writes data out to DAC's data register
 * 
 *	@param[in] u16_data The data to write
 *
 ****************************************************************************************************/
void DAC_write(uint16_t u16_data)
{
    u16_data &= DAC_MAXVAL;

    DAC_REGS->DAC_DATA = u16_data;

    while (DAC_REGS->DAC_SYNCBUSY & DAC_SYNCBUSY_DATABUF(1))
    {
        continue;
    }
}

/****************************************************************************************************
 *	Shell utility
 *
 * 	Performs a ramp test
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t DAC_shell_ramp(uint8_t argc, char ** argv)
{
	UNUSED(argv);

	if (argc == 0)
	{
		for (uint16_t i = 0; i < DAC_MAXVAL; i++)
		{
			DAC_write(i);
			CHRONO_delay_ms(1);
		}
		for (uint16_t i = DAC_MAXVAL; i > 0; i--)
		{
			DAC_write(i);
			CHRONO_delay_ms(1);
		}	
	}
	else
	{
		SHELL_printf("Usage: dac ramp\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

/****************************************************************************************************
 *	Shell utility
 *
 * 	Manually write 10 bits of data to DAC's DATA register
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t DAC_shell_write(uint8_t argc, char ** argv)
{
	uint32_t	u32_data;
	bool		b_res = false;

	if (argc == 1)
	{
		if (UTILS_string_to_u32(argv[0], &u32_data))
		{
			if (u32_data <= DAC_MAXVAL)
			{
				DAC_write(u32_data);
				b_res = true;
			}
		}
	}

	if (!b_res)
	{
		SHELL_printf("Usage: dac write <val>\n");
	}

	return SHELL_COMMAND_SUCCESS;
}
