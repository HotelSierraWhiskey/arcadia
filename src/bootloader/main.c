#include "memory_map.h"
#include <stdint.h>
#include "sys.h"
#include "uart.h"


static void __attribute__((naked)) start_app(uint32_t pc, uint32_t sp, uint32_t vtor)
{
	(void)pc;
	(void)sp;
	(void)vtor;
	
	__asm__ volatile(
		"ldr r3, =0xE000ED08 \n"
		"str r2, [r3]        \n"   // load SCB->VTOR into scratch register R3
		"msr msp, r1         \n"   // MSP = sp
		"cpsie i             \n"   // enable interrupts
		"bx  r0              \n"   // branch to pc
	);
}

void irqHARD_FAULT(void)
{
	while(1);
}

int main(void)
{
	// UART_init(UART_CHANNEL_SHELL);

	// UART_tx_char(UART_CHANNEL_SHELL, 'P');
	// UART_tx_char(UART_CHANNEL_SHELL, 'E');
	// UART_tx_char(UART_CHANNEL_SHELL, 'N');
	// UART_tx_char(UART_CHANNEL_SHELL, 'I');
	// UART_tx_char(UART_CHANNEL_SHELL, 'S');
	// UART_tx_char(UART_CHANNEL_SHELL, '\n');

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
