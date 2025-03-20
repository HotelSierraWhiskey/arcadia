#include "shell.h"
#include "utils.h"
#include "uart.h"
#include "sys.h"
#include "timer.h"
#include "nvmctrl.h"
#include "drive_api.h"
#include "chrono_api.h"
#include "chrono.h"
#include "spi.h"
#include "sd.h"
#include "app_fsm.h"
#include "io.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define SHELL_LOG_DBG(fmt, ...)   					SHELL_printf("\r%-12s" fmt, "[SHELL]", ##__VA_ARGS__)
#define SHELL_LOG_WARN(fmt, ...)   					SHELL_PRINT_WARNING("\r%-12s" fmt, "[SHELL]", ##__VA_ARGS__)

#define SHELL_COMMAND_BUFFER_SIZE	(512)
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

static void 	SHELL_flush_buffer			(void);
static void 	SHELL_handle_msg			(void);
static void 	SHELL_handle_esc_sequence	(void);
static void 	SHELL_handle_command		(void);
static void 	SHELL_help					(const SHELL_command_t * p_table);

uint8_t 		SHELL_shell_help			(uint8_t argc, char ** argv);

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

// Arcadia ASCII art banner
const char * kp_arcadia_banner = 
	"\n"
	SHELL_COLOR_VAPORWAVE_PINK		"   _                  _ _      \n"
	SHELL_COLOR_VAPORWAVE_PURPLE	"  /_\\  _ _ __ __ _ __| (_)__ _ \n"
	SHELL_COLOR_VAPORWAVE_AQUA		" / _ \\| '_/ _/ _` / _` | / _` |\n"
	SHELL_COLOR_VAPORWAVE_MINT		"/_/ \\_\\_| \\__\\__,_\\__,_|_\\__,_|\n"
	SHELL_COLOR_RESET;

// Top-level command table
static const SHELL_command_t kp_command_table[];

// CHRONO command table
static const SHELL_command_t kp_chrono_command_table[];

// DRIVE command tables
static const SHELL_command_t kp_drive_command_table[];
static const SHELL_command_t kp_drive_fs_command_table[];
static const SHELL_command_t kp_drive_nvm_command_table[];

// NVM command tables
static const SHELL_command_t kp_nvm_command_table[];

// SYS command tables
static const SHELL_command_t kp_sys_command_table[];

// TIMER command tables
static const SHELL_command_t kp_timer_command_table[];

// SD command tables
static const SHELL_command_t kp_sd_command_table[];

// SPI command tables
static const SHELL_command_t kp_spi_command_table[];

// UART command tables
static const SHELL_command_t kp_uart_command_table[];

// IO command tables
static const SHELL_command_t kp_io_command_table[];

/**
 *	Top level commands
 */
static const SHELL_command_t kp_command_table[] =
{
	{
		.kpc_name 			= "cat",
		.function 			= DRIVE_API_shell_cat,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tDumps the contents of a file\n"
									"\tUsage: cat <fname>\n"
								),
	},
	{
		.kpc_name 			= "cd",
		.function 			= DRIVE_API_shell_cd,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tChange the current working directory\n"
									"\tUsage: cd <dir>\n"
								),
	},
	{
		.kpc_name 			= "chrono",
		.function 			= NULL,
		.kp_command_table 	= kp_chrono_command_table,
		.kpc_docstring		= NULL
	},
	{
		.kpc_name 			= "drive",
		.function 			= NULL,
		.kp_command_table 	= kp_drive_command_table,
		.kpc_docstring		= NULL
	},
	{
		.kpc_name 			= "help",
		.function 			= SHELL_shell_help,
		.kp_command_table 	= NULL,
		.kpc_docstring		=	(
									"\tDisplays this message\n"
								)
	},
	{
		.kpc_name 			= "ls",
		.function 			= DRIVE_API_shell_ls,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tList file system contents\n"
									"\tUsage: ls\n"
								)
	},
	{
		.kpc_name 			= "pwd",
		.function 			= DRIVE_API_shell_pwd,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tDisplays the current working directory\n"
									"\tUsage: pwd\n"
								)
	},
	{
		.kpc_name 			= "nvm",
		.function 			= NULL,
		.kp_command_table 	= kp_nvm_command_table,
		.kpc_docstring		= NULL
	},
	{
		.kpc_name 			= "rm",
		.function 			= DRIVE_API_shell_rm,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tDeletes a file\n"
									"\tUsage: rm <fname>\n"
								)
	},
	{
		.kpc_name 			= "sd",
		.function 			= NULL,
		.kp_command_table 	= kp_sd_command_table,
		.kpc_docstring		= NULL
	},
	{
		.kpc_name 			= "spi",
		.function 			= NULL,
		.kp_command_table 	= kp_spi_command_table,
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
		.kpc_name 			= "touch",
		.function 			= DRIVE_API_shell_touch,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tCreates a empty file\n"
									"\tUsage: touch <fname>\n"
								)
	},
	{
		.kpc_name 			= "uart",
		.function 			= NULL,
		.kp_command_table 	= kp_uart_command_table,
		.kpc_docstring		= NULL
	},
	{
		.kpc_name 			= "io",
		.function 			= NULL,
		.kp_command_table 	= kp_io_command_table,
		.kpc_docstring		= NULL
	},
	//////////
	SHELL_COMMAND_TABLE_END
};

/**
 *	`chrono` commands
 */
static const SHELL_command_t kp_chrono_command_table[] =
{
	{
		.kpc_name 			= "cancel",
		.function 			= CHRONO_API_shell_cancel,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tCancels a scheduled message\n"
									"\tUsage: chrono cancel <timer_id>\n"
								)
	},
	{
		.kpc_name 			= "info",
		.function 			= CHRONO_API_shell_info,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tDisplays chrono msg schedule\n"
									"\tUsage: chrono info\n"
								)
	},
	{
		.kpc_name 			= "sn",
		.function 			= CHRONO_API_shell_sn,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tSchedules a NOOP for a given task\n"
									"\tUsage: chrono sn <task_id> <delay> <mode>\n"
								)
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
		.kpc_name 			= "fs",
		.function 			= NULL,
		.kp_command_table 	= kp_drive_fs_command_table,
		.kpc_docstring		= NULL
	},
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
 *	`drive fs` commands
 */
static const SHELL_command_t kp_drive_fs_command_table[] =
{
	{
		.kpc_name 			= "close",
		.function 			= DRIVE_API_shell_close,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tCloses a file\n"
									"\tUsage: drive fs close <fname>\n"
								)
	},
	{
		.kpc_name 			= "format",
		.function 			= DRIVE_API_shell_mkfs,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tRuns FatFs f_mkfs, creates the file system\n"
									"\tUsage: drive fs format\n"
								)
	},
	{
		.kpc_name 			= "info",
		.function 			= DRIVE_API_shell_fs_info,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tDisplays file system information\n"
									"\tUsage: drive fs info\n"
								)
	},
	{
		.kpc_name 			= "mount",
		.function 			= DRIVE_API_shell_mount,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tRuns FatFs f_mount, mounts the file system\n"
									"\tUsage: drive fs mount\n"
								)
	},
	{
		.kpc_name 			= "open",
		.function 			= DRIVE_API_shell_open,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tOpens a file\n"
									"\tUsage: drive fs open <fname>\n"
								)
	},
	{
		.kpc_name 			= "read",
		.function 			= DRIVE_API_shell_read,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tRuns FatFs f_mount, mounts the file system\n"
									"\tUsage: drive mount\n"
								)
	},
	{
		.kpc_name 			= "unmount",
		.function 			= DRIVE_API_shell_unmount,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tUnmounts the file system\n"
									"\tUsage: drive fs unmount\n"
								)
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
									"\tErases a row from NVM\n"
									"\tUsage: drive nvm erase <addr>\n"
								)
	},
	{
		.kpc_name 			= "read",
		.function 			= DRIVE_API_shell_read_nvm,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tReads a page from NVM\n"
									"\tUsage: drive nvm read <addr>\n"
								)
	},
	{
		.kpc_name 			= "write",
		.function 			= DRIVE_API_shell_write_nvm,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tErases a row from NVM\n"
									"\tUsage: drive nvm write <addr> <num_bytes> <...>\n"
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
									"\tErases a row from NVM\n"
									"\tUsage: nvm erase <addr>\n"
								)
	},
	{
		.kpc_name 			= "read",
		.function 			= NVMCTRL_shell_read,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tReads a page from NVM\n"
									"\tUsage: nvm read <addr>\n"
								)
	},
	{
		.kpc_name 			= "write",
		.function 			= NVMCTRL_shell_write,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tWrites a page to NVM\n"
									"\tUsage: nvm write <addr> <num_bytes> <...>\n"
								)
	},
	//////////
	SHELL_COMMAND_TABLE_END
};

/**
 *	`sd` commands
 */
static const SHELL_command_t kp_sd_command_table[] =
{
	{
		.kpc_name 			= "erase",
		.function 			= SD_shell_erase,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tErases a block of memory\n"
									"\tUsage: sd erase <addr>\n"
								)
	},
	{
		.kpc_name 			= "info",
		.function 			= SD_shell_info,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tDisplays SD info\n"
									"\tUsage: sd info\n"
								)
	},
	{
		.kpc_name 			= "init",
		.function 			= SD_shell_init,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tInitializes the SD card\n"
									"\tUsage: sd init\n"
								)
	},
	{
		.kpc_name 			= "read",
		.function 			= SD_shell_read,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tRead a block of data\n"
									"\tUsage: sd read <addr>\n"
								)
	},
	{
		.kpc_name 			= "wtest",
		.function 			= SD_shell_wtest,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tWrite a block of dummy data\n"
									"\tUsage: sd wtest <addr>\n"
								)
	},
	{
		.kpc_name 			= "wipe",
		.function 			= SD_shell_wipe,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tFills the SD card with zeros\n"
									"\tUsage: sd wipe\n"
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
									"\tForces a Hard Fault exception\n"
									"\tUsage: sys crash\n"
								)
	},
	{
		.kpc_name 			= "delay",
		.function 			= SYS_shell_delay,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tBlocking systick delay\n"
									"\tUsage: sys delay <ms>\n"
								)
	},
	{
		.kpc_name 			= "info",
		.function 			= SYS_shell_info,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tGeneral system information\n"
									"\tUsage: sys info\n"
								)
	},
	{
		.kpc_name 			= "reset",
		.function 			= SYS_shell_reset,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tPerforms a software reset\n"
									"\tUsage: sys reset\n"
								)
	},
	{
		.kpc_name 			= "wm",
		.function 			= SYS_shell_wm,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tView task high watermarks\n"
									"\tUsage: sys wm\n"
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
									"\tDisplays timer information\n"
									"\tUsage: timer info\n"
								)
	},
	{
		.kpc_name 			= "start",
		.function 			= TIMER_shell_start_timer,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tStarts a specified timer\n"
									"\tUsage: timer start <id> <seconds> <mode>\n"
								)
	},
	{
		.kpc_name 			= "stop",
		.function 			= TIMER_shell_stop_timer,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tStops a specified timer\n"
									"\tUsage: timer stop <id>\n"
								)
	},
	//////////
	SHELL_COMMAND_TABLE_END
};

static const SHELL_command_t kp_spi_command_table[] =
{
	{
		.kpc_name 			= "write",
		.function 			= SPI_shell_write,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tWrites a byte to the SPI interface\n"
									"\tUsage: spi write <channel_id> <num_bytes> <...>\n"
								)
	},
	{
		.kpc_name 			= "info",
		.function 			= SPI_shell_info,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tDisplay logical SPI channel info\n"
									"\tUsage: spi info\n"
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
									"\tDisplays UART configuration\n"
									"\tUsage: uart info\n"
								)
	},
	//////////
	SHELL_COMMAND_TABLE_END
};

/**
 *	`io` commands
 */
static const SHELL_command_t kp_io_command_table[] =
{
	{
		.kpc_name 			= "map",
		.function 			= IO_shell_map,
		.kp_command_table 	= NULL,
		.kpc_docstring		= 	(
									"\tDisplays IO pin map\n"
									"\tUsage: io map\n"
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

	APP_FSM_init();

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
		// Block and wait for a notification from the debug SERCOM's ISR or from a message notification
		if (ulTaskNotifyTake(pdFALSE, portMAX_DELAY) != 0)
		{
			SHELL_handle_msg();

			c = UART_rx_char(UART_CHANNEL_SHELL);

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

			else if (c == '\033')
			{
				SHELL_handle_esc_sequence();
				continue;
			}

			else if (c == '[')
			{
				APP_FSM_handle_event(FSM_EVENT_BUTTON_A_PRESSED);
				continue;
			}
			else if (c == ']')
			{
				APP_FSM_handle_event(FSM_EVENT_BUTTON_B_PRESSED);
				continue;
			}
			else if (c == '\\')
			{
				APP_FSM_handle_event(FSM_EVENT_BUTTON_MENU_PRESSED);
				continue;
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
    va_list args;

    va_start(args, format);

    xSemaphoreTake(SHELL_info.printf_mutex, portMAX_DELAY);

    (void)vsnprintf(printf_buffer, sizeof(printf_buffer), format, args);

    va_end(args);

    for (char *p = printf_buffer; *p != '\0'; ++p)
    {
        if (*p == '\n')  // Check for newline
        {
            UART_tx_char(UART_CHANNEL_SHELL, '\r');
        }
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

static void SHELL_handle_msg(void)
{
	ARCADIA_msg_t msg;

	if (ARCADIA_receive_nb(&msg))
	{
		SHELL_LOG_DBG("Received msg %s from %s\n", 
			ARCADIA_get_msg_type(msg.id), ARCADIA_get_task_name(msg.from));

		switch (msg.id)
		{
			case ARCADIA_MSG_ID_NOOP:
				break;
			
			default:
				SHELL_LOG_DBG("Unexpected message: %u\n", msg.id);
		}
	}
}

static void SHELL_handle_esc_sequence(void)
{
	char esc_seq[3];

	// UART_rx_char is non blocking and will immediately return the current character in its buffer.
	// We could be reading the buffer before the character arrives, which is bad. So, add a slight delay here.
	CHRONO_delay_ms(2);

	esc_seq[0] = UART_rx_char(UART_CHANNEL_SHELL);
	esc_seq[1] = UART_rx_char(UART_CHANNEL_SHELL);
	esc_seq[2] = '\0';

	if (esc_seq[0] == '[')
	{
		switch (esc_seq[1])
		{
			case 'A':
				APP_FSM_handle_event(FSM_EVENT_BUTTON_UP_PRESSED);
				break;
			case 'B':
				APP_FSM_handle_event(FSM_EVENT_BUTTON_DOWN_PRESSED);
				break;
			case 'C':
				APP_FSM_handle_event(FSM_EVENT_BUTTON_RIGHT_PRESSED);
				break;
			case 'D':
				APP_FSM_handle_event(FSM_EVENT_BUTTON_LEFT_PRESSED);
				break;
			default:
				break;
		}
	}
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
					SHELL_printf("\nToo many arguments\n");
					goto cleanup;
				}
			}
			else
			{
				SHELL_printf("\nCommand not found: %s\n", token);
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
			SHELL_printf("\n");
		}

		if (shell_function(argc, argv) != SHELL_COMMAND_SUCCESS)
		{
			SHELL_printf("\nCommand returned bad status code\n");
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
	SHELL_printf("\n\nCommands:\n");

	SHELL_SEPARATOR();

	while (p_table->kpc_name)
	{
		SHELL_printf("%s\n", p_table->kpc_name);

		// Check if a docstring exists for this command
		if (p_table->kpc_docstring)
		{
			SHELL_printf("%s", p_table->kpc_docstring);
		}
		else
		{
			SHELL_printf("\t%s commands\n", p_table->kpc_name);
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
	SHELL_printf("%s\n", kp_arcadia_banner);
}

/****************************************************************************************************
 *	S H E L L   F U N C T I O N S
 ****************************************************************************************************/

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
