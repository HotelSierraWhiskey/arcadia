#ifndef LOAD_PAYLOAD_H
#define LOAD_PAYLOAD_H

#include "common.h"
#include "arcadia_status.h"
#include "fsif.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

typedef struct _LOAD_PAYLOAD_load_module
{
	file_handle_t			fh;
	char *					pc_name;
	ARCADIA_status_t *		p_result_status;
} LOAD_PAYLOAD_load_module_t;

#endif // LOAD_PAYLOAD_H
