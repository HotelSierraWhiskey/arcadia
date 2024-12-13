#include "shell.h"
#include "uart.h"
#include "sys.h"
#include "nvmctrl.h"

#include <FreeRTOS.h>
#include "semphr.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define SHELL_COMMAND_BUFFER_SIZE	(16)
#define SHELL_CRLF					"\r\n"
#define SHELL_MAX_TOKENS			(256)
#define SHELL_MAX_ARGS				(128)
#define SHELL_COMMAND_TABLE_END		{NULL, NULL, NULL}

typedef uint8_t (* SHELL_function_t)(uint8_t argc, char ** argv);

typedef struct _SHELL_command
{
	const char *					kpc_name;
	SHELL_function_t				function;
	const struct _SHELL_command * 	kp_command_table;
	const char *					kpc_docstring;
} SHELL_command_t;

typedef struct _SHELL_info
{
	char		pc_buffer[SHELL_COMMAND_BUFFER_SIZE];
	uint16_t	u16_index;
} SHELL_info_t;

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void 	SHELL_flush_buffer		(void);
static void 	SHELL_handle_command	(void);
static void		SHELL_display_banner	(void);
static void 	SHELL_help				(const SHELL_command_t * p_table);

uint8_t 		SHELL_shell_help		(uint8_t argc, char ** argv);

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

SemaphoreHandle_t xPrintfMutex;

/**
 *	`nvm` commands
 */
static const SHELL_command_t kp_nvm_command_table[] =
{
	{
		.kpc_name 			= "erase",
		.function 			= NVMCTRL_shell_erase,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tErases a row from NVM\r\n"
									"\tUsage: uart info\r\n"
								)
	},
	{
		.kpc_name 			= "read",
		.function 			= NVMCTRL_shell_read,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tReads a page from NVM\r\n"
									"\tUsage: uart info\r\n"
								)
	},
	{
		.kpc_name 			= "write",
		.function 			= NVMCTRL_shell_write,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tWrites a page to NVM\r\n"
									"\tUsage: uart info\r\n"
								)
	},
	//////////
	SHELL_COMMAND_TABLE_END
};

/**
 *	`sys` commands
 */
static const SHELL_command_t kp_sys_command_table[] =
{
	{
		.kpc_name 			= "crash",
		.function 			= SYS_shell_crash,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tForces a Hard Fault exception\r\n"
									"\tUsage: sys crash\r\n"
								)
	},
	{
		.kpc_name 			= "delay",
		.function 			= SYS_shell_delay,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tBlocking systick delay\r\n"
									"\tUsage: sys delay <ms>\r\n"
								)
	},
	{
		.kpc_name 			= "info",
		.function 			= SYS_shell_info,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tGeneral system information\r\n"
									"\tUsage: sys info\r\n"
								)
	},
	{
		.kpc_name 			= "reset",
		.function 			= SYS_shell_reset,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tPerforms a software reset\r\n"
									"\tUsage: sys reset\r\n"
								)
	},
	//////////
	SHELL_COMMAND_TABLE_END
};

/**
 *	`uart` commands
 */
static const SHELL_command_t kp_uart_command_table[] =
{
	{
		.kpc_name 			= "info",
		.function 			= UART_shell_info,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tDisplays UART configuration\r\n"
									"\tUsage: uart info\r\n"
								)
	},
	//////////
	SHELL_COMMAND_TABLE_END
};

/**
 *	Top level commands
 */
static const SHELL_command_t kp_command_table[] =
{
	{
		.kpc_name 			= "help",
		.function 			= SHELL_shell_help,
		.kp_command_table 	= NULL,
		.kpc_docstring		=	(
									"\tDisplays this message\r\n"
								)
	},
	{
		.kpc_name 			= "nvm",
		.function 			= NULL,
		.kp_command_table 	= kp_nvm_command_table,
		.kpc_docstring		= NULL
	},
	{
		.kpc_name 			= "sys",
		.function 			= NULL,
		.kp_command_table 	= kp_sys_command_table,
		.kpc_docstring		= NULL
	},
	{
		.kpc_name 			= "uart",
		.function 			= NULL,
		.kp_command_table 	= kp_uart_command_table,
		.kpc_docstring		= NULL
	},
	//////////
	SHELL_COMMAND_TABLE_END
};

static SHELL_info_t SHELL_info;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void SHELL_init(void)
{
	UART_init(UART_CHANNEL_SHELL);
	SHELL_flush_buffer();

	xPrintfMutex = xSemaphoreCreateMutex();

	// SHELL_display_banner();

	// SHELL_printf("%s", SHELL_PROMPT);
}

void SHELL_task(void * p_params)
{
	(void)p_params;

	// SHELL_init();

	while (1)
	{
		// SHELL_printf("poopoo\n\r");
		// vTaskDelay(100);
		char c = UART_rx_char(UART_CHANNEL_SHELL);

		// Returned zero, nothing to do
		if (!c)
		{
			continue;
		}

		// Handle return key
		if (c == '\r')
		{
			if (strlen(SHELL_info.pc_buffer) == 0)
			{
				SHELL_printf("\r\n%s", SHELL_PROMPT);
			}
			else
			{
				SHELL_handle_command();
				SHELL_printf("%s", SHELL_PROMPT);
			}

			SHELL_flush_buffer();
		}

		// Handle delete
		else if (c == '\b' || c == 0x7F)
		{
			if (SHELL_info.u16_index > 0)
			{
				// Move the cursor back, overwrite that character with a space, then move back again
				SHELL_printf("\b \b");
				
				// Null the last character
				SHELL_info.pc_buffer[--SHELL_info.u16_index] = '\0';
			}
			// return;
		}

		// Push the char onto the buffer and echo
		else
		{
			SHELL_info.pc_buffer[SHELL_info.u16_index++] = c;
			SHELL_printf("%c", c);
		}
	}
}

char		buffer[SHELL_COMMAND_BUFFER_SIZE];

void SHELL_printf(const char *format, ...)
{
	xSemaphoreTake(xPrintfMutex, portMAX_DELAY);

	va_list 	args;

	va_start(args, format);

	(void)vsnprintf(buffer, sizeof(buffer), format, args);

	va_end(args);

	for (char *p = buffer; *p != '\0'; ++p)
	{
		UART_tx_char(UART_CHANNEL_SHELL, *p);
	}

	xSemaphoreGive(xPrintfMutex);
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
	SHELL_command_t *	p_table = (SHELL_command_t *)kp_command_table;
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
				if (p_table[i].kp_command_table != NULL)
				{
					p_table = (SHELL_command_t *)p_table[i].kp_command_table;
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
					SHELL_flush_buffer();
					return;
				}
			}
			else
			{
				SHELL_printf("\r\nCommand not found: %s\r\n", token);
				SHELL_flush_buffer();
				return;
			}
		}

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
	// Issue help on that table
	else
	{
		SHELL_help(p_table);
	}

	SHELL_flush_buffer();
}

static void SHELL_help(const SHELL_command_t * p_table)
{
	SHELL_printf("\r\n\nCommands:\r\n");

	SHELL_SEPARATOR();

	while (p_table->kpc_name)
	{
		SHELL_printf("%s\r\n", p_table->kpc_name);

		// Check if a docstring exists for this command
		if (p_table->kpc_docstring)
		{
			SHELL_printf("%s", p_table->kpc_docstring);
		}
		else
		{
			SHELL_printf("\t%s commands\r\n", p_table->kpc_name);
		}
		p_table++;
	}

	SHELL_SEPARATOR();
}

static void SHELL_display_banner(void)
{
	SHELL_printf("\r\n");
	SHELL_printf(SHELL_COLOR_VAPORWAVE_PINK);
	SHELL_printf("   _                  _ _      \n\r");
	SHELL_printf(SHELL_COLOR_VAPORWAVE_PURPLE);
	SHELL_printf("  /_\\  _ _ __ __ _ __| (_)__ _ \n\r");
	SHELL_printf(SHELL_COLOR_VAPORWAVE_AQUA);
	SHELL_printf(" / _ \\| '_/ _/ _` / _` | / _` |\n\r");
	SHELL_printf(SHELL_COLOR_VAPORWAVE_MINT);
	SHELL_printf("/_/ \\_\\_| \\__\\__,_\\__,_|_\\__,_|\n\r");
	SHELL_printf(SHELL_COLOR_RESET);
	SHELL_printf("\r\n");
}

uint8_t SHELL_shell_help(uint8_t argc, char ** argv)
{
	// Just issue help on the top-level command table
	SHELL_help(kp_command_table);

	return SHELL_COMMAND_SUCCESS;
}
