#include "uart.h"
#include "shell_types.h"
#include "shell_printf.h"

static char printf_buffer[SHELL_COMMAND_BUFFER_SIZE];

SHELL_info_t SHELL_info;

/****************************************************************************************************
 *	Shell's `printf` implementation, used system-wide
 *
 ****************************************************************************************************/
void SHELL_printf(const char *format, ...)
{
	va_list args;

	va_start(args, format);

	PRINTF_SEMAPHORE_TAKE();

	(void)vsnprintf(printf_buffer, sizeof(printf_buffer), format, args);

	va_end(args);

	printf("%s", printf_buffer);

	PRINTF_SEMAPHORE_GIVE();
}
