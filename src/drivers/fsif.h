#ifndef FSIF_H
#define FSIF_H

#include "common.h"
#include "ff.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define FSIF_INVALID_OPEN_MODE	(-1)
#define FSIF_INVALID_FILE		(-1)

typedef FIL file_t;
typedef int8_t file_handle_t;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

bool 			FSIF_fs_init					(void);
FRESULT 		FSIF_f_mkfs						(void);
FRESULT 		FSIF_f_mount					(void);
int32_t 		FSIF_open_mode_from_posix_flag	(const char *kpc_posix_flag);
const char * 	FSIF_get_fat_subtype			(void);
const char * 	FSIF_get_volume_label			(void);

#endif // FSIF_H
