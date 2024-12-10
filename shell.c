#include "shell.h"
#include "uart.h"
#include "sys.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define SHELL_COMMAND_BUFFER_SIZE	(512)
#define SHELL_PROMPT				"> "
#define SHELL_CRLF					"\r\n"
#define SHELL_MAX_TOKENS			(20)
#define SHELL_MAX_ARGS				(5)
#define SHELL_COMMAND_TABLE_END		{NULL, NULL, NULL}
#define SHELL_DOCSTRING(str)		str"\r\n"

typedef uint8_t (* SHELL_function_t)(uint8_t argc, char ** argv);

typedef struct _SHELL_command
{
	const char *				kpc_name;
	SHELL_function_t			function;
	struct _SHELL_command * 	p_command_table;
	const char *				kpc_docstring;
} SHELL_command_t;

typedef struct _SHELL_info
{
	char		pc_buffer[SHELL_COMMAND_BUFFER_SIZE];
	uint16_t	u16_index;
} SHELL_info_t;

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

static const SHELL_command_t kp_sys_command_table[] =
{
	{
		.kpc_name 			= "info",
		.function 			= SYS_shell_info,
		.p_command_table 	= NULL,
		.kpc_docstring		= SHELL_DOCSTRING
								(
									"\t\tGeneral system information\r\n"
								)
	},
	//////////
	SHELL_COMMAND_TABLE_END
};

static const SHELL_command_t kp_uart_command_table[] =
{
	{
		.kpc_name 			= "info",
		.function 			= UART_shell_info,
		.p_command_table 	= NULL,
		.kpc_docstring		= SHELL_DOCSTRING
								(
									"\t\tDisplays UART configuration\r\n"
									"\t\tUsage: uart info\r\n"
								)
	},
	//////////
	SHELL_COMMAND_TABLE_END
};

static const SHELL_command_t kp_command_table[] =
{
	{
		.kpc_name 			= "sys",
		.function 			= NULL,
		.p_command_table 	= kp_sys_command_table,
		.kpc_docstring		= NULL
	},
	{
		.kpc_name 			= "uart",
		.function 			= NULL,
		.p_command_table 	= kp_uart_command_table,
		.kpc_docstring		= NULL
	},
	//////////
	SHELL_COMMAND_TABLE_END
};

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

	SHELL_printf("\r\n");
	SHELL_printf("   _                  _ _      \n\r");
	SHELL_printf("  /_\\  _ _ __ __ _ __| (_)__ _ \n\r");
	SHELL_printf(" / _ \\| '_/ _/ _` / _` | / _` |\n\r");
	SHELL_printf("/_/ \\_\\_| \\__\\__,_\\__,_|_\\__,_|\n\r");        
	SHELL_printf("\r\n");

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
		if (strncmp(SHELL_info.pc_buffer, strlen(SHELL_CRLF), 2) != 0)
		{
			SHELL_handle_command();
			SHELL_printf("%s", SHELL_PROMPT);
		}
		else
		{
			SHELL_printf("\r\n%s", SHELL_PROMPT);
		}

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
			SHELL_info.pc_buffer[--SHELL_info.u16_index] = '\0';
		}
		return;
	}

	// Push the char onto the buffer and echo
	else
	{
		SHELL_info.pc_buffer[SHELL_info.u16_index++] = c;
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
	memset(SHELL_info.pc_buffer, 0, SHELL_COMMAND_BUFFER_SIZE);
	SHELL_info.u16_index = 0;
}

static void SHELL_handle_command(void)
{
	uint8_t 			argc = 0;
	char *				argv[SHELL_MAX_ARGS];
	SHELL_command_t *	p_table = kp_command_table;
	SHELL_function_t 	shell_function = NULL;
	bool 				b_found = false;

	// Tokenize the input command buffer
	char *token = strtok(SHELL_info.pc_buffer, " ");
	while (token != NULL)
	{
		// Search for the token in the current command table
		b_found = false;
		for (uint8_t i = 0; p_table[i].kpc_name != NULL; i++)
		{
			if (strcmp(p_table[i].kpc_name, token) == 0)
			{
				// If the command has a subcommand table, traverse into it
				if (p_table[i].p_command_table != NULL)
				{
					p_table = p_table[i].p_command_table;
					b_found = true;
					break;
				}
				// If the command has an associated function, set it
				else if (p_table[i].function != NULL)
				{
					shell_function = p_table[i].function;
					b_found = true;
					break;
				}
			}
		}

		// If the token was not found in the current table
		if (!b_found)
		{
			// If no command or subcommand matched, treat as an argument
			if (shell_function)
			{
				argv[argc++] = token;
				
				if (argc >= SHELL_MAX_ARGS)
				{
					SHELL_printf("\r\nToo many arguments\r\n");
					return;
				}
			}
			else
			{
				SHELL_printf("\r\nCommand not found: %s\r\n", token);
				return;
			}
		}

		// Get the next token
		token = strtok(NULL, " ");
	}

	// Execute the function if found
	if (shell_function)
	{
		SHELL_printf("\r\n");

		if (shell_function(argc, argv) != SHELL_COMMAND_SUCCESS)
		{
			SHELL_printf("\r\nCommand returned bad status code\r\n");
		}
	}
	// We found a command table but stopped short of invoking one of its functions
	// Issue help
	else
	{
		SHELL_printf("\r\n\nCommands:\r\n");

		while (p_table->kpc_name)
		{
			SHELL_printf("\t%s\r\n", p_table->kpc_name);

			// Check if a docstring exists for this command
			if (p_table->kpc_docstring)
			{
				SHELL_printf("%s", p_table->kpc_docstring);
			}
			p_table++;
		}
	}
	SHELL_flush_buffer();
}
