#ifndef FSIF_H
#define FSIF_H

#include "common.h"
#include "ff.h"

bool 			FSIF_fs_init			(void);
FRESULT 		FSIF_f_mkfs				(void);
FRESULT 		FSIF_f_mount			(void);
const char * 	FSIF_get_fat_subtype	(void);
const char * 	FSIF_get_volume_label	(void);

#endif // FSIF_H
