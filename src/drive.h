#ifndef DRIVE_H
#define DRIVE_H

#include "common.h"
#include "fsif.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define DRIVE_INVALID_HANDLE (-1)

typedef int8_t DRIVE_file_handle_t;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void		DRIVE_init						(void);
void		DRIVE_task						(void * p_params);
file_t * 	DRIVE_index_to_file_pointer		(uint8_t u8_index);
int8_t 		DRIVE_file_pointer_to_index		(file_t * p_file);

#endif // DRIVE_H
