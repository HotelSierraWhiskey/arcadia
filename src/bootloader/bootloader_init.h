#ifndef BOOTLOADER_INIT_H
#define BOOTLOADER_INIT_H

#include <stdint.h>

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define BOOTLOADER_INIT_NAKED	__attribute__((naked))

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void							BOOTLOADER_INIT_sys_clock_init			(void);
void BOOTLOADER_INIT_NAKED		BOOTLOADER_INIT_start_app				(uint32_t u32_pc, uint32_t u32_sp, uint32_t u32_vtor);

#endif // BOOTLOADER_INIT_H
