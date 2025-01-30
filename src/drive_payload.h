#ifndef DRIVE_PAYLOAD_H
#define DRIVE_PAYLOAD_H

#include "common.h"
#include "arcadia_status.h"
#include "fsif.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

typedef struct _DRIVE_PAYLOAD_write_nvm
{
	uint32_t				u32_addr;
	const char *			kpc_buffer;
	ARCADIA_status_t *		p_result_status;
} DRIVE_PAYLOAD_write_nvm_t;

typedef struct _DRIVE_PAYLOAD_read_nvm
{
	uint32_t				u32_addr;
	char *					pc_buffer;
	ARCADIA_status_t *		p_result_status;
} DRIVE_PAYLOAD_read_nvm_t;

typedef struct _DRIVE_PAYLOAD_erase_nvm
{
	uint32_t				u32_addr;
	ARCADIA_status_t *		p_result_status;
} DRIVE_PAYLOAD_erase_nvm_t;

typedef struct _DRIVE_PAYLOAD_open_file
{
	file_handle_t * 		p_file_handle;
	const char *			kpc_fname;
	const char *			kpc_open_mode;
	ARCADIA_status_t *		p_result_status;
} DRIVE_PAYLOAD_open_file_t;

typedef struct _DRIVE_PAYLOAD_close_file
{
	file_handle_t 			file_handle;
	ARCADIA_status_t *		p_result_status;
} DRIVE_PAYLOAD_close_file_t;

typedef struct _DRIVE_PAYLOAD_fetch_fnames
{
	uint8_t					u8_num_fnames;
	const char *			kpc_filter;
	uint8_t 				u8_start_index;
	char ** 				ppc_buffer;
	uint8_t * 				pu8_num_found;
	ARCADIA_status_t *		p_result_status;
} DRIVE_PAYLOAD_fetch_fnames_t;

typedef struct _DRIVE_PAYLOAD_chdir
{
	const char *			kpc_dirname;
	ARCADIA_status_t *		p_result_status;
} DRIVE_PAYLOAD_chdir_t;

typedef struct _DRIVE_PAYLOAD_write
{
	file_handle_t 			file_handle;
	const char *			kpc_data;
	ARCADIA_status_t *		p_result_status;
} DRIVE_PAYLOAD_write_t;

typedef struct _DRIVE_PAYLOAD_read
{
	file_handle_t 			file_handle;
	char *					pc_data;
	uint32_t				u32_bytes_to_read;
	ARCADIA_status_t *		p_result_status;
} DRIVE_PAYLOAD_read_t;

#endif // DRIVE_PAYLOAD_H
