#include "shell.h"
#include "uart.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define SHELL_COMMAND_BUFFER_SIZE	(512)
#define SHELL_PROMPT				"> "

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

static char 	SHELL_read			(void);
static void 	SHELL_flush_buffer	(void);


/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void SHELL_init(void)
{
	UART_init(UART_CHANNEL_SHELL);
	SHELL_flush_buffer();
}

void SHELL_run(void)
{
	char c = UART_rx_char(UART_CHANNEL_SHELL);

	// if (c == '\r')
	// {
	// 	SHELL_printf("Entered command: %s\n", SHELL_info.buffer);
	// 	SHELL_flush_buffer();
	// }
	// else
	// {
	// 	SHELL_info.buffer[SHELL_info.u16_index++] = c;
	// }
	SHELL_printf("%c", c);
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