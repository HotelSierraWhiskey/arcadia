#ifndef DRIVE_API_H
#define DRIVE_API_H

#include "common.h"
#include "arcadia.h"
#include "nvmctrl.h"

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

ARCADIA_status_t	DRIVE_API_read_nvm			(const NVMCTRL_app_nvm_row_id_t row_id, char * pc_data);
ARCADIA_status_t	DRIVE_API_write_nvm			(const NVMCTRL_app_nvm_row_id_t row_id, const char * kpc_data);
ARCADIA_status_t	DRIVE_API_erase_nvm			(const NVMCTRL_app_nvm_row_id_t row_id);

/****************************************************************************************************
 *	S H E L L   F U N C T I O N S
 ****************************************************************************************************/

uint8_t				DRIVE_API_shell_erase_nvm	(uint8_t argc, char ** argv);
uint8_t				DRIVE_API_shell_read_nvm	(uint8_t argc, char ** argv);
uint8_t				DRIVE_API_shell_write_nvm	(uint8_t argc, char ** argv);

#endif // DRIVE_API_H