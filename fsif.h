#ifndef FSIF_H
#define FSIF_H

#include "ff.h"

void 		FSIF_fs_init	(void);
FRESULT 	FSIF_f_mkfs		(void);
FRESULT 	FSIF_f_mount	(void);
FRESULT 	FSIF_f_open		(const char * kpc_fname, char * buf, uint32_t * bw, uint32_t * br);
FRESULT 	FSIF_f_read		(const char * kpc_fname);

#endif // FSIF_H
