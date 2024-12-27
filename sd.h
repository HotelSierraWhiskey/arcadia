#ifndef SD_H
#define SD_H

#include "common.h"

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void 		SD_card_init			(void);
uint8_t 	SD_write_block			(uint32_t u32_block_address, const uint8_t * kpu8_buffer);
uint8_t 	SD_read_block			(uint32_t u32_block_address, uint8_t * pu8_buffer);

/****************************************************************************************************
 *	S H E L L   F U N C T I O N S
 ****************************************************************************************************/

uint8_t		SD_shell_read			(uint8_t argc, char ** argv);
uint8_t 	SD_shell_wtest			(uint8_t argc, char ** argv);

#endif // SD_H
