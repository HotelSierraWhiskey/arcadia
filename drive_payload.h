#ifndef DRIVE_PAYLOAD_H
#define DRIVE_PAYLOAD_H

#include "common.h"
#include "arcadia_status.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

typedef struct _DRIVE_PAYLOAD_write_nvm
{
	uint32_t			u32_addr;
	char *				pc_buffer;
} DRIVE_PAYLOAD_write_nvm_t;

typedef struct _DRIVE_PAYLOAD_read_nvm
{
	uint32_t			u32_addr;
	char *				pc_buffer;
	ARCADIA_status_t *	p_result_status;
} DRIVE_PAYLOAD_read_nvm_t;

typedef struct _DRIVE_PAYLOAD_erase_nvm
{
	uint32_t	u32_addr;
} DRIVE_PAYLOAD_erase_nvm_t;

#endif // DRIVE_PAYLOAD_H