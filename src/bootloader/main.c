#include "memory_map.h"
#include <stdint.h>
#include "sys.h"
#include "io.h"
#include "uart.h"
#include "bootloader.h"
#include "printf.h"
#include "shell_utils.h"

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

int NORETURN main(void)
{
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Warray-bounds"

	uint32_t *	pu8_interrupt_table = &_approm_start;
	uint32_t 	u32_app_stack_pointer = pu8_interrupt_table[0];
	uint32_t 	u32_app_reset_handler = pu8_interrupt_table[1];

	#pragma GCC diagnostic pop

	// global IRQ disable
	__disable_irq();

	// osc, system clocks, uart, etc.
	BOOTLOADER_init();
	
	BOOT_CLEAR_TERMINAL();
	BOOT_LOG_DBG("System init\n");

	if (BOOTLOADER_update_flag_set())
	{
		BOOT_LOG_DBG("Starting firmware update...\n");
		BOOTLOADER_update_firmware();
		BOOT_LOG_DBG("Done.");
	}

	// jump to app
	BOOTLOADER_start_app(u32_app_reset_handler, u32_app_stack_pointer, (uint32_t)pu8_interrupt_table);

	// not reached
	while(1);
}
