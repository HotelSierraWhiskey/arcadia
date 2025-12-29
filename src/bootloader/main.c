#include "memory_map.h"
#include <stdint.h>
#include "sys.h"
#include "io.h"
#include "uart.h"


static void __attribute__((naked)) start_app(uint32_t pc, uint32_t sp, uint32_t vtor)
{
	(void)pc;
	(void)sp;
	(void)vtor;
	
	__asm__ volatile(
		"ldr r3, =0xE000ED08	\n"
		"str r2, [r3]			\n"	// load SCB->VTOR into scratch register R3
		"msr msp, r1			\n"	// MSP = sp
		"cpsie i				\n"	// enable interrupts
		"bx  r0					\n"	// branch to pc
	);
}

void BOOT_init(void)
{
	// SYS_osc48m_init()
	/* ************************** */

	// Two wait states are required to run at 48MHz
	NVMCTRL_REGS->NVMCTRL_CTRLB = NVMCTRL_CTRLB_RWS_DUAL;

	// Enable in on-demand mode with a division factor of 1 (for 48MHz) with 21.33us startup delay
	OSCCTRL_REGS->OSCCTRL_OSC48MCTRL = 	OSCCTRL_OSC48MCTRL_ENABLE(1) |
										OSCCTRL_OSC48MCTRL_ONDEMAND(1);
	OSCCTRL_REGS->OSCCTRL_OSC48MDIV = 0;
	OSCCTRL_REGS->OSCCTRL_OSC48MSTUP = OSCCTRL_OSC48MSTUP_STARTUP_CYCLE1024;

	// Wait for synchronization
	while (OSCCTRL_REGS->OSCCTRL_OSC48MSYNCBUSY & OSCCTRL_OSC48MSYNCBUSY_OSC48MDIV(1))
	{
		continue;
	}

	// Stabilize
	while ((OSCCTRL_REGS->OSCCTRL_STATUS & OSCCTRL_STATUS_OSC48MRDY(1)) == 0)
	{
		continue;
	}

	// SYS_clock_init()
	/* ************************** */

	// Enable clock ready interrupt
	MCLK_REGS->MCLK_INTENSET = MCLK_INTENSET_CKRDY(1);

	// Configure GCLK0 with OSC48M as a clock source
	GCLK_REGS->GCLK_GENCTRL[0] = GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_OSC48M) | 
                                 GCLK_GENCTRL_GENEN(1) |
								 GCLK_GENCTRL_DIVSEL(0) |
								 GCLK_GENCTRL_DIV(0) |
								 GCLK_GENCTRL_IDC(1) |
                                 GCLK_GENCTRL_OE(1);

	while (GCLK_REGS->GCLK_SYNCBUSY & GCLK_SYNCBUSY_GENCTRL0(1))
	{
		continue;
	}

	// Division factor of one for the main clock. This is the reset/ default value
	MCLK_REGS->MCLK_CPUDIV = MCLK_CPUDIV_CPUDIV_DIV1;

	// Wait for the clock to be ready
	while ((MCLK_REGS->MCLK_INTFLAG & MCLK_INTFLAG_CKRDY(1)) == 0)
	{
		continue;
	}

	// // Configure GCLK1 with OSC32K as a clock source
	// GCLK_REGS->GCLK_GENCTRL[1] = 	GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_OSC32K) |
	// 							 	GCLK_GENCTRL_DIV(1) |
	// 								GCLK_GENCTRL_OE(1) |
	// 							 	GCLK_GENCTRL_IDC(1) |
	// 								GCLK_GENCTRL_GENEN(1);

	// Enable peripheral clock for reset controller
	MCLK_REGS->MCLK_APBAMASK |= MCLK_APBAMASK_RSTC(1);

	/* ************************** */
									 
		// Set the APB for the PORT peripheral
	MCLK_REGS->MCLK_APBBMASK |= MCLK_APBBMASK_PORT(1);
}

void irqHARD_FAULT(void)
{
	while(1);
}

int main(void)
{
	__disable_irq();

	BOOT_init();
	
	UART_init(UART_CHANNEL_SHELL);
	UART_tx_string_raw(UART_CHANNEL_SHELL, "System boot\r\n");

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Warray-bounds"

	uint32_t *	pu8_interrupt_table  = &_approm_start;
	uint32_t 	u32_app_stack_pointer = pu8_interrupt_table[0];
	uint32_t 	u32_app_reset_handler = pu8_interrupt_table[1];

	#pragma GCC diagnostic pop

	start_app(u32_app_reset_handler, u32_app_stack_pointer, (uint32_t)pu8_interrupt_table);

	// not reached
	while(1)
	{
		continue;
	}
}
