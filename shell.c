#include "shell.h"
#include "uart.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define SHELL_COMMAND_BUFFER_SIZE	(512)
#define SHELL_PROMPT				"> "
#define SHELL_CRLF					"\r\n"

typedef struct _SHELL_command
{
	const char *				kpc_name;
	SHELL_function_t			function;
	struct _SHELL_command * 	p_command_table;

} SHELL_command_t;

typedef struct _SHELL_info
{
	char		buffer[SHELL_COMMAND_BUFFER_SIZE];
	uint16_t	u16_index;
} SHELL_info_t;

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

static SHELL_info_t SHELL_info;

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static char 	SHELL_read				(void);
static void 	SHELL_flush_buffer		(void);
static void 	SHELL_handle_command	(void);

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void SHELL_init(void)
{
	UART_init(UART_CHANNEL_SHELL);
	SHELL_flush_buffer();
	SHELL_printf("%s", SHELL_PROMPT);
}

void SHELL_run(void)
{
	char c = UART_rx_char(UART_CHANNEL_SHELL);

	// Returned zero, nothing to do
	if (!c)
	{
		return;
	}

	// Handle return
	if (c == '\r')
	{
		if (strncmp(SHELL_info.buffer, strlen(SHELL_CRLF), 2) != 0)
		{
			SHELL_handle_command();
		}

		SHELL_printf("\r\n%s", SHELL_PROMPT);
		SHELL_flush_buffer();
	}

	// Handle delete
	else if (c == '\b' || c == 0x7F)
	{
		if (SHELL_info.u16_index > 0)
		{
			// Move the cursor back, overwrite the character with a space, then move back again
			SHELL_printf("\b \b");
			
			// Null the last character
			SHELL_info.buffer[--SHELL_info.u16_index] = '\0';
		}
		return;
	}

	// Push the char onto the buffer and echo
	else
	{
		SHELL_info.buffer[SHELL_info.u16_index++] = c;
		SHELL_printf("%c", c);
	}
}

void SHELL_printf(const char *format, ...)
{
	char		buffer[SHELL_COMMAND_BUFFER_SIZE];
	va_list 	args;

	va_start(args, format);

	int len = vsnprintf(buffer, sizeof(buffer), format, args);

	if (len < 0 || len >= (int)sizeof(buffer))
	{
		buffer[sizeof(buffer) - 1] = '\0';
	}

	va_end(args);

	for (char *p = buffer; *p != '\0'; ++p)
	{
		UART_tx_char(UART_CHANNEL_SHELL, *p);
	}
}

static char SHELL_read(void)
{
	return UART_rx_char(UART_CHANNEL_SHELL);
}

static void SHELL_flush_buffer(void)
{
	memset(SHELL_info.buffer, 0, SHELL_COMMAND_BUFFER_SIZE);
	SHELL_info.u16_index = 0;
}

static void SHELL_handle_command(void)
{
	SHELL_printf("\r\nCommand not found: %s", SHELL_info.buffer);
}
