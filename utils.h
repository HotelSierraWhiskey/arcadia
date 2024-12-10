#ifndef UTILS_H
#define UTILS_H

#include "common.h"
#include "shell.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define NORETURN		__attribute__((noreturn))
#define WEAKREF			__attribute__((weak))

#define ASSERT(x) do { \
	if (!(x)) \
	{ \
		SHELL_PRINT_ERROR("Assertion failed. %s:%u\r\n", __FILE__, __LINE__); \
		UTILS_error_crash(); \
	} \
} while (0)

void UTILS_error_crash(void);

#endif // UTILS_H
