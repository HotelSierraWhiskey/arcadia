#ifndef FSIF_H
#define FSIF_H

#include "common.h"
#include "ff.h"

bool 			FSIF_fs_init			(void);
FRESULT 		FSIF_f_mkfs				(void);
FRESULT 		FSIF_f_mount			(void);
FRESULT 		FSIF_f_open				(const char * kpc_fname, char * buf, uint32_t * bw, uint32_t * br);
FRESULT 		FSIF_f_read				(const char * kpc_fname);
void 			FSIF_f_ls				(void);
FATFS * 		FSIF_f_get_fs			(void);
const char * 	FSIF_get_fat_subtype	(void);
void 			FSIF_get_volume_label	(void);

#endif // FSIF_H
