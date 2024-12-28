#ifndef UTILS_H
#define UTILS_H

#include "common.h"
#include "shell.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define NORETURN			__attribute__((noreturn))
#define WEAKREF				__attribute__((weak))
#define SECTION_APP_NVM		__attribute__((section(".app_nvm")))
#define PACKED				__attribute__((packed, aligned))
#define UNUSED(x)			(void)(x)

#define ASSERT(x) do { \
	if (!(x)) \
	{ \
		SHELL_PRINT_ERROR("Assertion failed. %s:%u\r\n", __FILE__, __LINE__); \
		UTILS_error_crash(); \
	} \
} while (0)

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void	UTILS_error_crash		(void);
bool	UTILS_string_to_u32		(const char *kpc_str, uint32_t *u32_out);

#endif // UTILS_H
