#include "shell.h"
#include "utils.h"
#include "uart.h"
#include "sys.h"
#include "nvmctrl.h"
#include "drive_api.h"
#include "timer.h"

#include "chrono.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define SHELL_LOG_DBG(fmt, ...)   	SHELL_printf("%-10s" fmt, "[SHELL]", ##__VA_ARGS__)

#define SHELL_COMMAND_BUFFER_SIZE	(128)
#define SHELL_CRLF					"\r\n"
#define SHELL_MAX_TOKENS			(16)
#define SHELL_MAX_ARGS				(8)
#define SHELL_COMMAND_TABLE_END		{NULL, NULL, NULL, NULL}

/**
 *	Shell function pointer typedef
 */
typedef uint8_t (* SHELL_function_t)(uint8_t argc, char ** argv);

/**
 *	Shell command entry
 */
typedef struct _SHELL_command
{
	const char *					kpc_name;
	SHELL_function_t				function;
	const struct _SHELL_command * 	kp_command_table;
	const char *					kpc_docstring;
} SHELL_command_t;

/**
 *	Module info struct
 */
typedef struct _SHELL_info
{
	char				pc_buffer[SHELL_COMMAND_BUFFER_SIZE];
	uint16_t			u16_index;
	SemaphoreHandle_t 	printf_mutex;
	StaticSemaphore_t 	printf_mutex_buffer;
} SHELL_info_t;

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void 	SHELL_flush_buffer		(void);
static void 	SHELL_handle_command	(void);
static void 	SHELL_help				(const SHELL_command_t * p_table);

uint8_t 		SHELL_shell_help		(uint8_t argc, char ** argv);

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

// Top-level command table
static const SHELL_command_t kp_command_table[];

// DRIVE command tables
static const SHELL_command_t kp_drive_command_table[];
static const SHELL_command_t kp_drive_nvm_command_table[];

// NVM command tables
static const SHELL_command_t kp_nvm_command_table[];

// SYS command tables
static const SHELL_command_t kp_sys_command_table[];

// TIMER command tables
static const SHELL_command_t kp_timer_command_table[];

// UART command tables
static const SHELL_command_t kp_uart_command_table[];

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
		.kpc_name 			= "drive",
		.function 			= NULL,
		.kp_command_table 	= kp_drive_command_table,
		.kpc_docstring		= NULL
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
		.kpc_name 			= "timer",
		.function 			= NULL,
		.kp_command_table 	= kp_timer_command_table,
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

/**
 *	`drive` commands
 */
static const SHELL_command_t kp_drive_command_table[] =
{
	{
		.kpc_name 			= "nvm",
		.function 			= NULL,
		.kp_command_table 	= kp_drive_nvm_command_table,
		.kpc_docstring		= NULL
	},
	//////////
	SHELL_COMMAND_TABLE_END
};

/**
 *	`drive nvm` commands
 */
static const SHELL_command_t kp_drive_nvm_command_table[] =
{
	{
		.kpc_name 			= "erase",
		.function 			= DRIVE_API_shell_erase_nvm,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tErases a row from NVM\r\n"
									"\tUsage: drive nvm erase <addr>\r\n"
								)
	},
	{
		.kpc_name 			= "read",
		.function 			= DRIVE_API_shell_read_nvm,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tReads a page from NVM\r\n"
									"\tUsage: drive nvm read <addr>\r\n"
								)
	},
	{
		.kpc_name 			= "write",
		.function 			= DRIVE_API_shell_write_nvm,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tErases a row from NVM\r\n"
									"\tUsage: drive nvm write <addr> <num_bytes> <...>\r\n"
								)
	},
	//////////
	SHELL_COMMAND_TABLE_END
};

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
									"\tUsage: nvm erase <addr>\r\n"
								)
	},
	{
		.kpc_name 			= "read",
		.function 			= NVMCTRL_shell_read,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tReads a page from NVM\r\n"
									"\tUsage: nvm read <addr>\r\n"
								)
	},
	{
		.kpc_name 			= "write",
		.function 			= NVMCTRL_shell_write,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tWrites a page to NVM\r\n"
									"\tUsage: nvm write <addr> <num_bytes> <...>\r\n"
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
	{
		.kpc_name 			= "wm",
		.function 			= SYS_shell_wm,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tView task high watermarks\r\n"
									"\tUsage: sys wm\r\n"
								)
	},
	//////////
	SHELL_COMMAND_TABLE_END
};

/**
 *	`timer` commands
 */
static const SHELL_command_t kp_timer_command_table[] =
{
	{
		.kpc_name 			= "info",
		.function 			= TIMER_shell_info,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tDisplays timer information\r\n"
									"\tUsage: timer info\r\n"
								)
	},
	{
		.kpc_name 			= "start",
		.function 			= TIMER_shell_start_timer,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tStarts a specified timer\r\n"
									"\tUsage: timer start <id> <seconds> <mode>\r\n"
								)
	},
	{
		.kpc_name 			= "stop",
		.function 			= TIMER_shell_stop_timer,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tStops a specified timer\r\n"
									"\tUsage: timer stop <id>\r\n"
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

static char printf_buffer[SHELL_COMMAND_BUFFER_SIZE];

static SHELL_info_t SHELL_info;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	Pre-kernel module initialization function
 *
 ****************************************************************************************************/
void SHELL_init(void)
{
	UART_init(UART_CHANNEL_SHELL);
	SHELL_flush_buffer();

	SHELL_info.printf_mutex = xSemaphoreCreateMutexStatic(&SHELL_info.printf_mutex_buffer);

	SHELL_display_banner();

	SHELL_printf("%s", SHELL_PROMPT);
}

/****************************************************************************************************
 *	Top level task loop
 *
 ****************************************************************************************************/
void SHELL_task(void * p_params)
{
	UNUSED(p_params);

	char c;

	while (1)
	{
		if (ulTaskNotifyTake(pdFALSE, portMAX_DELAY) != 0)
		{
			c = UART_rx_char(UART_CHANNEL_SHELL);

			// Returned zero, nothing to do, let someone else do some work
			if (!c)
			{
				CHRONO_delay_ms(1);
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

			// Handle backspace/ delete keys
			else if (c == '\b' || c == 0x7F)
			{
				if (SHELL_info.u16_index > 0)
				{
					// Move the cursor back, overwrite that character with a space, then move back again
					SHELL_printf("\b \b");
					
					// Null the last character
					SHELL_info.pc_buffer[--SHELL_info.u16_index] = '\0';
				}
			}

			// Push the char onto the buffer and echo
			else
			{
				SHELL_info.pc_buffer[SHELL_info.u16_index++] = c;
				SHELL_printf("%c", c);
			}
		}
	}
}

/****************************************************************************************************
 *	Shell's `printf` implementation, used system-wide
 *
 ****************************************************************************************************/
void SHELL_printf(const char *format, ...)
{
	xSemaphoreTake(SHELL_info.printf_mutex, portMAX_DELAY);

	va_list 	args;

	va_start(args, format);

	(void)vsnprintf(printf_buffer, sizeof(printf_buffer), format, args);

	va_end(args);

	for (char *p = printf_buffer; *p != '\0'; ++p)
	{
		UART_tx_char(UART_CHANNEL_SHELL, *p);
	}

	xSemaphoreGive(SHELL_info.printf_mutex);
}

/****************************************************************************************************
 *	Flushes the shell command buffer
 *
 ****************************************************************************************************/
static void SHELL_flush_buffer(void)
{
	memset(SHELL_info.pc_buffer, 0, SHELL_COMMAND_BUFFER_SIZE);
	SHELL_info.u16_index = 0;
}

/****************************************************************************************************
 *	Processes the the shell command buffer when a command is entered in the serial debug interface
 *
 ****************************************************************************************************/
static void SHELL_handle_command(void)
{
	uint8_t argc = 0;
	char *argv[SHELL_MAX_ARGS];
	SHELL_command_t *p_table = (SHELL_command_t *)kp_command_table;
	SHELL_function_t shell_function = NULL;

	// Tokenize the input command buffer
	char *token = strtok(SHELL_info.pc_buffer, " ");
	while (token != NULL)
	{
		bool command_found = false;

		// Search for the token in the current command table
		for (uint8_t i = 0; p_table[i].kpc_name != NULL; i++)
		{
			if (strcmp(p_table[i].kpc_name, token) == 0)
			{
				if (p_table[i].kp_command_table != NULL)
				{
					// Navigate to subcommand table
					p_table = (SHELL_command_t *)p_table[i].kp_command_table;
				}
				else if (p_table[i].function != NULL)
				{
					// Set the function to execute
					shell_function = p_table[i].function;
				}
				command_found = true;
				break;
			}
		}

		// Handle unrecognized token
		if (!command_found)
		{
			if (shell_function)
			{
				argv[argc++] = token;

				if (argc >= SHELL_MAX_ARGS)
				{
					SHELL_printf("\r\nToo many arguments\r\n");
					goto cleanup;
				}
			}
			else
			{
				SHELL_printf("\r\nCommand not found: %s\r\n", token);
				goto cleanup;
			}
		}

		token = strtok(NULL, " ");
	}

	// Execute the command or provide help
	if (shell_function)
	{
		// Keep spacing uniform when top-level help is called
		if (shell_function != SHELL_shell_help)
		{
			SHELL_printf("\r\n");
		}

		if (shell_function(argc, argv) != SHELL_COMMAND_SUCCESS)
		{
			SHELL_printf("\r\nCommand returned bad status code\r\n");
		}
	}
	else
	{
		SHELL_help(p_table);
	}

cleanup:
	SHELL_flush_buffer();
	vPortYield();
}

/****************************************************************************************************
 *	Displays the commands and docs associated with a given command table
 *
 * 	@param[in] p_table The command table to view
 *
 ****************************************************************************************************/
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

/****************************************************************************************************
 *	Displays the Arcadia banner
 *
 ****************************************************************************************************/
void SHELL_display_banner(void)
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

/****************************************************************************************************
 *	Shell utility
 *
 * 	Runs `SHELL_help` on the top-level command table
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t SHELL_shell_help(uint8_t argc, char ** argv)
{
	// Just issue help on the top-level command table
	SHELL_help(kp_command_table);

	return SHELL_COMMAND_SUCCESS;
}
