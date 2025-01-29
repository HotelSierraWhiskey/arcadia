#ifndef DRIVE_API_H
#define DRIVE_API_H

#include "common.h"
#include "arcadia.h"
#include "nvmctrl.h"
#include "fsif.h"

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

ARCADIA_status_t	DRIVE_API_read_nvm			(const NVMCTRL_app_nvm_row_id_t row_id, char * pc_data);
ARCADIA_status_t	DRIVE_API_write_nvm			(const NVMCTRL_app_nvm_row_id_t row_id, const char * kpc_data);
ARCADIA_status_t	DRIVE_API_erase_nvm			(const NVMCTRL_app_nvm_row_id_t row_id);

ARCADIA_status_t	DRIVE_API_open_file			(file_t * p_file, const char *kpc_fname, const char * kpc_open_mode);
ARCADIA_status_t	DRIVE_API_close_file		(file_t * p_file);
ARCADIA_status_t 	DRIVE_API_chdir				(const char * kpc_dirname);
ARCADIA_status_t 	DRIVE_API_fetch_fnames		(uint8_t u8_num_fnames, const char * kpc_filter, uint8_t u8_start_index, char ** ppc_buffer, uint8_t * pu8_num_found);

/****************************************************************************************************
 *	S H E L L   F U N C T I O N S
 ****************************************************************************************************/

// NVM shell commands
uint8_t				DRIVE_API_shell_erase_nvm	(uint8_t argc, char ** argv);
uint8_t				DRIVE_API_shell_read_nvm	(uint8_t argc, char ** argv);
uint8_t				DRIVE_API_shell_write_nvm	(uint8_t argc, char ** argv);

// FS shell commands
uint8_t 			DRIVE_API_shell_cat			(uint8_t argc, char ** argv);
uint8_t 			DRIVE_API_shell_cd			(uint8_t argc, char ** argv);
uint8_t 			DRIVE_API_shell_close 		(uint8_t argc, char ** argv);
uint8_t 			DRIVE_API_shell_fs_info		(uint8_t argc, char ** argv);
uint8_t 			DRIVE_API_shell_ls			(uint8_t argc, char ** argv);
uint8_t 			DRIVE_API_shell_mount		(uint8_t argc, char ** argv);
uint8_t 			DRIVE_API_shell_mkfs		(uint8_t argc, char ** argv);
uint8_t 			DRIVE_API_shell_open 		(uint8_t argc, char ** argv);
uint8_t 			DRIVE_API_shell_pwd			(uint8_t argc, char ** argv);
uint8_t 			DRIVE_API_shell_read 		(uint8_t argc, char ** argv);
uint8_t 			DRIVE_API_shell_rm			(uint8_t argc, char ** argv);
uint8_t 			DRIVE_API_shell_touch		(uint8_t argc, char ** argv);
uint8_t 			DRIVE_API_shell_unmount		(uint8_t argc, char ** argv);

#endif // DRIVE_API_H
