#ifndef SYS_H
#define SYS_H

#include "common.h"
#include "utils.h"

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void 				SYS_init 					(void);
uint32_t			SYS_get_source_clock_freq 	(void);
void				SYS_reset					(void);

uint8_t				SYS_shell_crash				(uint8_t argc, char ** argv);
uint8_t 			SYS_shell_delay				(uint8_t argc, char ** argv);
uint8_t 			SYS_shell_info				(uint8_t argc, char ** argv);
uint8_t				SYS_shell_reset				(uint8_t argc, char ** argv);

#endif // SYS_H
