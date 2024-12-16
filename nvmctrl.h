#ifndef NVMCTRL_H
#define NVMCTRL_H

#include "common.h"
#include "shell.h"

#define NVMCTRL_MEMORY				((volatile uint16_t * )(0x00U))

void		NVMCTRL_init			(void);
void 		NVMCTRL_write_page		(uint32_t u32_addr, uint8_t * pu8_buffer);
void 		NVMCTRL_erase_row		(uint32_t u32_addr);

uint8_t		NVMCTRL_shell_erase		(uint8_t argc, char ** argv);
uint8_t		NVMCTRL_shell_read		(uint8_t argc, char ** argv);
uint8_t		NVMCTRL_shell_write		(uint8_t argc, char ** argv);



#endif // NVMCTRL_H
