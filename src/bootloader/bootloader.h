#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include "common.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define BOOT_LOG_DBG(fmt, ...)   		tfp_printf("\r%-12s" fmt, "[BOOT]", ##__VA_ARGS__)

#define BOOT_CLEAR_TERMINAL()			tfp_printf(SHELL_CLEAR_SCREEN)

#define NAKED							__attribute__((naked))
#define NORETURN						__attribute__((noreturn))

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void						BOOTLOADER_init					(void);
void NAKED NORETURN			BOOTLOADER_start_app			(uint32_t u32_pc, uint32_t u32_sp, uint32_t u32_vtor);
bool 						BOOTLOADER_update_flag_set		(void);
void						BOOTLOADER_update_firmware		(void);

#endif // BOOTLOADER_H
