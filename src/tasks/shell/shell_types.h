#ifndef SHELL_TYPES_H
#define SHELL_TYPES_H

#include "common.h"

#ifdef BOOTLOADER
#define PRINTF_SEMAPHORE_TAKE()
#define PRINTF_SEMAPHORE_GIVE()
#else
#define PRINTF_SEMAPHORE_TAKE()		xSemaphoreTake(SHELL_info.printf_mutex, portMAX_DELAY)
#define PRINTF_SEMAPHORE_GIVE()		xSemaphoreGive(SHELL_info.printf_mutex)
#endif // BOOTLOADER

#define SHELL_COMMAND_BUFFER_SIZE	(512)

/**
 *	Module info struct
 */
typedef struct _SHELL_info
{
	char				pc_buffer[SHELL_COMMAND_BUFFER_SIZE];
	uint16_t			u16_index;
#ifndef BOOTLOADER	
	SemaphoreHandle_t 	printf_mutex;
	StaticSemaphore_t 	printf_mutex_buffer;
#endif // BOOTLOADER
} SHELL_info_t;

#endif // SHELL_TYPES_H
