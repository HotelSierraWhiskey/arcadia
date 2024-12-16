#ifndef DRIVE_API_H
#define DRIVE_API_H

#include "common.h"
#include "arcadia.h"

ARCADIA_status_t	DRIVE_API_read_nvm		(const uint32_t u32_addr, char * pc_data);
ARCADIA_status_t	DRIVE_API_write_nvm		(const uint32_t u32_addr, char * pc_data);
ARCADIA_status_t	DRIVE_API_erase_nvm		(const uint32_t u32_addr);

#endif // DRIVE_API_H