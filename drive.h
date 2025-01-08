#ifndef DRIVE_H
#define DRIVE_H

#include "common.h"
#include "drive_payload.h"

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void		DRIVE_init					(void);
void		DRIVE_task					(void * p_params);
file_t * 	DRIVE_index_to_file_pointer	(uint8_t u8_index);

#endif // DRIVE_H