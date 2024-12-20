#include "chrono.h"
#include "io.h"
#include "sys.h"
#include "shell.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define SYS_TICK_FREQ_1MS(source_clock_freq) ((source_clock_freq) / (1000U))

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void		CHRONO_init		(void);

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void vPortSetupTimerInterrupt(void)
{
	CHRONO_init();
}

static void CHRONO_init(void)
{
	uint32_t u32_source_clock_freq = SYS_get_source_clock_freq();
	SysTick_Config(SYS_TICK_FREQ_1MS(u32_source_clock_freq));

	IO_config_pin_direction(IO_PIN_ID_PA27, IO_DIRECTION_OUTPUT);

	GCLK_REGS->GCLK_PCHCTRL[30] = 	GCLK_PCHCTRL_CHEN(1) | 
									GCLK_PCHCTRL_GEN_GCLK0;

	MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_TC0(1);

	TC0_REGS->COUNT16.TC_INTENSET = TC_INTENSET_MC0(1);
	// TC0_REGS->COUNT16.TC_INTENSET = TC_INTENSET_OVF(1);

	// TC0_REGS->COUNT16.TC_CC[0] = 1000;
	// TC0_REGS->COUNT16.TC_CCBUF[0] = 1001;

	TC0_REGS->COUNT16.TC_COUNT = 1000;
	
	TC0_REGS->COUNT16.TC_CTRLA = 	TC_CTRLA_MODE_COUNT16 |
									// TC_CTRLA_PRESCALER_DIV1024 |
									TC_CTRLA_CAPTEN0(1) |
									TC_CTRLA_DIR(1) |
									TC_CTRLA_ENABLE(1);

	SHELL_printf("Current timer count: %lu\r\n", TC0_REGS->COUNT16.TC_COUNT);
	SHELL_printf("Interrupt flags: %lu\r\n", TC0_REGS->COUNT16.TC_INTFLAG);
	// SHELL_printf("TC_CC 0: %lu\r\n", TC0_REGS->COUNT16.TC_CC[0]);
	// SHELL_printf("TC_CCBUF 0: %lu\r\n", TC0_REGS->COUNT16.TC_CCBUF[0]);
	SHELL_printf("===\r\n");

	NVIC_EnableIRQ(TC0_IRQn);
}

uint32_t CHRONO_ticks_since(uint32_t start_ticks)
{
	uint32_t current_ticks = CHRONO_get_ticks();
	return current_ticks - start_ticks;
}

volatile bool s = false;

void irqTC0(void)
{
	if ((TC0_REGS->COUNT16.TC_INTFLAG & TC_INTFLAG_MC0(1)) != 0)
	{
		SHELL_printf("Current timer count: %lu\r\n", TC0_REGS->COUNT16.TC_COUNT);
		SHELL_printf("Interrupt flags: %lu\r\n", TC0_REGS->COUNT16.TC_INTFLAG);
		SHELL_printf("TC_CC 0: %lu\r\n", TC0_REGS->COUNT16.TC_CC[0]);
		SHELL_printf("TC_CCBUF 0: %lu\r\n", TC0_REGS->COUNT16.TC_CCBUF[0]);

		if (s)
		{
			IO_set_pin(IO_PIN_ID_PA27, IO_PIN_STATE_HIGH);
			s = false;
		}
		else
		{
			IO_set_pin(IO_PIN_ID_PA27, IO_PIN_STATE_LOW);
			s = true;
		}

		// uint32_t next_match = TC0_REGS->COUNT16.TC_COUNT + 1000;
        // TC0_REGS->COUNT16.TC_CCBUF[0] = next_match;
		TC0_REGS->COUNT16.TC_COUNT = 0;

		TC0_REGS->COUNT16.TC_INTFLAG = TC_INTFLAG_MC0(1);
	}

	// if ((TC0_REGS->COUNT16.TC_INTFLAG & TC_INTFLAG_OVF(1)) != 0)
	// {
	// 	if (s)
	// 	{
	// 		IO_set_pin(IO_PIN_ID_PA27, IO_PIN_STATE_HIGH);
	// 		s = false;
	// 	}
	// 	else
	// 	{
	// 		IO_set_pin(IO_PIN_ID_PA27, IO_PIN_STATE_LOW);
	// 		s = true;
	// 	}

	// 	TC0_REGS->COUNT16.TC_INTFLAG = TC_INTFLAG_OVF(1);
	// }

	NVIC_ClearPendingIRQ(TC0_IRQn);
}
