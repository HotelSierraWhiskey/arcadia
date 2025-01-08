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
	uint32_t			u32_addr;
	const char *		kpc_buffer;
	ARCADIA_status_t *	p_result_status;
} DRIVE_PAYLOAD_write_nvm_t;

typedef struct _DRIVE_PAYLOAD_read_nvm
{
	uint32_t			u32_addr;
	char *				pc_buffer;
	ARCADIA_status_t *	p_result_status;
} DRIVE_PAYLOAD_read_nvm_t;

typedef struct _DRIVE_PAYLOAD_erase_nvm
{
	uint32_t			u32_addr;
	ARCADIA_status_t *	p_result_status;
} DRIVE_PAYLOAD_erase_nvm_t;

typedef struct _DRIVE_PAYLOAD_open_file
{
	const char *		kpc_fname;
	file_t *			p_file;
	ARCADIA_status_t *	p_result_status;
} DRIVE_PAYLOAD_open_file_t;

typedef struct _DRIVE_PAYLOAD_close_file
{
	file_t *			p_file;
	ARCADIA_status_t *	p_result_status;
} DRIVE_PAYLOAD_close_file_t;

#endif // DRIVE_PAYLOAD_H