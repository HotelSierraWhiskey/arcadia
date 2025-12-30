#include "memory_map.h"
#include <stdint.h>
#include "sys.h"
#include "io.h"
#include "uart.h"
#include "bootloader_init.h"
#include "printf.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define BOOT_LOG_DBG(fmt, ...)   		tfp_printf("\r%-12s" fmt, "[BOOT]", ##__VA_ARGS__)

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

int main(void)
{
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Warray-bounds"

	uint32_t *	pu8_interrupt_table  = &_approm_start;
	uint32_t 	u32_app_stack_pointer = pu8_interrupt_table[0];
	uint32_t 	u32_app_reset_handler = pu8_interrupt_table[1];

	#pragma GCC diagnostic pop

	__disable_irq();

	BOOTLOADER_INIT_sys_clock_init();

	PRINTF_init();
	
	BOOT_LOG_DBG("System init\n");

	// jump to app
	BOOTLOADER_INIT_start_app(u32_app_reset_handler, u32_app_stack_pointer, (uint32_t)pu8_interrupt_table);

	// not reached
	while(1)
	{
		continue;
	}
}
