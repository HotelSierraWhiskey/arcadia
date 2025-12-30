#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include "common.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define BOOTLOADER_NAKED	__attribute__((naked))

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void						BOOTLOADER_init					(void);
void BOOTLOADER_NAKED		BOOTLOADER_start_app			(uint32_t u32_pc, uint32_t u32_sp, uint32_t u32_vtor);

#endif // BOOTLOADER_H
