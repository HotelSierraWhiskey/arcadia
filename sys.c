#include "common.h"
#include "sys.h"
#include "io.h"
#include "shell.h"
#include "chronos.h"
#include "version.h"
#include "arcadia.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define SYS_LOG_DBG(fmt, ...)   	SHELL_printf("%-10s" fmt, "[SYS]", ##__VA_ARGS__)

typedef enum _SYS_clock_src_freq
{
	SYS_CLOCK_SRC_FREQ_48_MHZ,
	SYS_CLOCK_SRC_FREQ_24_MHZ,
	SYS_CLOCK_SRC_FREQ_16_MHZ,
	SYS_CLOCK_SRC_FREQ_12_MHZ,
	SYS_CLOCK_SRC_FREQ_9_6_MHZ,
	SYS_CLOCK_SRC_FREQ_8_MHZ,
	SYS_CLOCK_SRC_FREQ_6_86_MHZ,
	SYS_CLOCK_SRC_FREQ_6_MHZ,
	SYS_CLOCK_SRC_FREQ_5_33_MHZ,
	SYS_CLOCK_SRC_FREQ_4_8_MHZ,
	SYS_CLOCK_SRC_FREQ_4_36_MHZ,
	SYS_CLOCK_SRC_FREQ_4_MHZ,
	SYS_CLOCK_SRC_FREQ_3_69_MHZ,
	SYS_CLOCK_SRC_FREQ_3_43_MHZ,
	SYS_CLOCK_SRC_FREQ_3_2_MHZ,
	SYS_CLOCK_SRC_FREQ_3_MHZ,
	//////////
	SYS_CLOCK_SRC_FREQ_NUM_FREQ
} SYS_clock_src_freq_t;

typedef enum _SYS_part
{
	SYS_PART_ATSAMC21E18A,
	//////////
	SYS_PART_NUM_PARTS
} SYS_part_t;

typedef struct _SYS_info
{
	SYS_part_t				k_part;
	SYS_clock_src_freq_t	clock_source_freq;
} SYS_info_t;

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

static const char * const kpc_sys_clock_freq_descriptors[SYS_CLOCK_SRC_FREQ_NUM_FREQ] = 
{
	[SYS_CLOCK_SRC_FREQ_48_MHZ] 	= "48 MHz",
	[SYS_CLOCK_SRC_FREQ_24_MHZ] 	= "24 MHz",
	[SYS_CLOCK_SRC_FREQ_16_MHZ] 	= "16 MHz",
	[SYS_CLOCK_SRC_FREQ_12_MHZ] 	= "12 MHz",
	[SYS_CLOCK_SRC_FREQ_9_6_MHZ] 	= "9.6 MHz",
	[SYS_CLOCK_SRC_FREQ_8_MHZ] 		= "8 MHz",
	[SYS_CLOCK_SRC_FREQ_6_86_MHZ] 	= "6.86 MHz",
	[SYS_CLOCK_SRC_FREQ_6_MHZ] 		= "6 MHz",
	[SYS_CLOCK_SRC_FREQ_5_33_MHZ] 	= "5.33 MHz",
	[SYS_CLOCK_SRC_FREQ_4_8_MHZ] 	= "4.8 MHz",
	[SYS_CLOCK_SRC_FREQ_4_36_MHZ] 	= "4.36 MHz",
	[SYS_CLOCK_SRC_FREQ_4_MHZ] 		= "4 MHz",
	[SYS_CLOCK_SRC_FREQ_3_69_MHZ] 	= "3.69 MHz",
	[SYS_CLOCK_SRC_FREQ_3_43_MHZ] 	= "3.43 MHz",
	[SYS_CLOCK_SRC_FREQ_3_2_MHZ] 	= "3.2 MHz",
	[SYS_CLOCK_SRC_FREQ_3_MHZ] 		= "3 MHz",
};

static const uint32_t kpu32_sys_clock_frequencies[SYS_CLOCK_SRC_FREQ_NUM_FREQ] = 
{
	[SYS_CLOCK_SRC_FREQ_48_MHZ]   = 48000000,
	[SYS_CLOCK_SRC_FREQ_24_MHZ]   = 24000000,
	[SYS_CLOCK_SRC_FREQ_16_MHZ]   = 16000000,
	[SYS_CLOCK_SRC_FREQ_12_MHZ]   = 12000000,
	[SYS_CLOCK_SRC_FREQ_9_6_MHZ]  = 9600000,
	[SYS_CLOCK_SRC_FREQ_8_MHZ]    = 8000000,
	[SYS_CLOCK_SRC_FREQ_6_86_MHZ] = 6860000,
	[SYS_CLOCK_SRC_FREQ_6_MHZ]    = 6000000,
	[SYS_CLOCK_SRC_FREQ_5_33_MHZ] = 5330000,
	[SYS_CLOCK_SRC_FREQ_4_8_MHZ]  = 4800000,
	[SYS_CLOCK_SRC_FREQ_4_36_MHZ] = 4360000,
	[SYS_CLOCK_SRC_FREQ_4_MHZ]    = 4000000,
	[SYS_CLOCK_SRC_FREQ_3_69_MHZ] = 3690000,
	[SYS_CLOCK_SRC_FREQ_3_43_MHZ] = 3430000,
	[SYS_CLOCK_SRC_FREQ_3_2_MHZ]  = 3200000,
	[SYS_CLOCK_SRC_FREQ_3_MHZ]    = 3000000,
};

static const char * const kpc_part_descriptors[SYS_PART_NUM_PARTS] = 
{
	[SYS_PART_ATSAMC21E18A] = "ATSAMC21E18A"
};

static SYS_info_t SYS_info;

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void 	SYS_clock_init			(void);
static void 	SYS_osc48m_init			(void);

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	Main system-level initialization
 *
 ****************************************************************************************************/
void SYS_init(void)
{
	SYS_info.k_part = SYS_PART_ATSAMC21E18A;

	SYS_osc48m_init();

	SYS_clock_init();

	IO_init();

	// Output the main clock signal on PA27
	// IO_enable_peripheral_function_for_pin(IO_PIN_ID_PA27, IO_PERIPHERAL_FUNCTION_H);
}

/****************************************************************************************************
 *	Initializes OSC48M (the internal 48MHz oscillator)
 * 
 * 	@todo tighten up the config interface/ SYS_info members here
 ****************************************************************************************************/
static void SYS_osc48m_init(void)
{
	// Two wait states are required to run at 48MHz
	NVMCTRL_REGS->NVMCTRL_CTRLB = NVMCTRL_CTRLB_RWS_DUAL;

	// Enable in on-demand mode with a division factor of 1 (for 48MHz) with 21.33us startup delay
	OSCCTRL_REGS->OSCCTRL_OSC48MCTRL = OSCCTRL_OSC48MCTRL_ENABLE(1) | OSCCTRL_OSC48MCTRL_ONDEMAND(1);
	OSCCTRL_REGS->OSCCTRL_OSC48MDIV = OSCCTRL_OSC48MDIV_DIV_DIV1;
	OSCCTRL_REGS->OSCCTRL_OSC48MSTUP = OSCCTRL_OSC48MSTUP_STARTUP_CYCLE1024;

	// Wait for synchronization
	while (OSCCTRL_REGS->OSCCTRL_OSC48MSYNCBUSY & OSCCTRL_OSC48MSYNCBUSY_OSC48MDIV(1))
	{
		continue;
	}

	// Stabilize
	while ((OSCCTRL_REGS->OSCCTRL_STATUS & OSCCTRL_STATUS_OSC48MRDY(1)) == 0)
	{
		continue;
	}

	SYS_info.clock_source_freq = SYS_CLOCK_SRC_FREQ_48_MHZ;
}

void SYS_reset(void)
{
	NVIC_SystemReset();
}

/****************************************************************************************************
 *	Clock initialization
 *
 ****************************************************************************************************/
static void SYS_clock_init(void)
{
	// Enable clock ready interrupt
	MCLK_REGS->MCLK_INTENSET = MCLK_INTENSET_CKRDY(1);

	// Provide GCLK0 with OSC48M as a clock source
	GCLK_REGS->GCLK_GENCTRL[0] = GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_OSC48M) | 
                                 GCLK_GENCTRL_GENEN(1) |
								 GCLK_GENCTRL_DIVSEL(0) |
								 GCLK_GENCTRL_DIV(0) |
								 GCLK_GENCTRL_IDC(1) |
                                 GCLK_GENCTRL_OE(1);

	// Division factor of one for the main clock. This is the reset/ default value
	MCLK_REGS->MCLK_CPUDIV = MCLK_CPUDIV_CPUDIV_DIV1;

	// Wait for the clock to be ready
	while ((MCLK_REGS->MCLK_INTFLAG & MCLK_INTFLAG_CKRDY(1)) == 0)
	{
		continue;
	}
}

uint32_t SYS_get_source_clock_freq(void)
{
	return kpu32_sys_clock_frequencies[SYS_info.clock_source_freq];
}

uint8_t SYS_shell_crash(uint8_t argc, char ** argv)
{
	if (argc == 0)
	{
		ASSERT(0);
	}
	else
	{
		SHELL_printf("Usage: sys crash\r\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

uint8_t SYS_shell_delay(uint8_t argc, char ** argv)
{
	bool b_res = false;
	uint32_t u32_delay;

	if (argc == 1)
	{
		if (UTILS_string_to_u32(argv[0], &u32_delay))
		{
			SYS_LOG_DBG("Delaying %u ms\r\n", u32_delay);
			CHRONOS_delay_ms(u32_delay);
			SYS_LOG_DBG("Done\r\n");

			b_res = true;
		}
	}

	if (!b_res)
	{
		SHELL_printf("Usage: sys delay <ms>\r\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

uint8_t SYS_shell_info(uint8_t argc, char ** argv)
{
	if (argc == 0)
	{
		// Uptime stuff
		uint32_t u32_uptime_s 	= CHRONOS_get_ticks() / 1000;
		uint32_t u32_hours 		= u32_uptime_s / 3600;
		uint32_t u32_minutes 	= (u32_uptime_s % 3600) / 60;
		uint32_t u32_seconds 	= u32_uptime_s % 60;
		char pc_time_buffer[12];
		sprintf(pc_time_buffer, "%02lu:%02lu:%02lu", u32_hours, u32_minutes, u32_seconds);

		// Serial number stuff
		uint32_t uid_buffer[4];
    	uid_buffer[0] = *(uint32_t *)0x0080A00C;
		uid_buffer[1] = *(uint32_t *)0x0080A040;
		uid_buffer[2] = *(uint32_t *)0x0080A044;
		uid_buffer[3] = *(uint32_t *)0x0080A048;

		char pc_serial_number[36];
		sprintf(pc_serial_number, "%08lX-%08lX-%08lX-%08lX", 
			uid_buffer[0], 
			uid_buffer[1], 
			uid_buffer[2], 
			uid_buffer[3]);

		SHELL_SEPARATOR();
		SHELL_printf("%-30s: %u.%u.%u\r\n", "Firmware Version", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
		SHELL_printf("%-30s: %s\r\n", "FreeRTOS Version", VERSION_FREERTOS);
		SHELL_printf("%-30s: %s %s\r\n", "Compilation Timestamp", __DATE__, __TIME__);
		SHELL_printf("%-30s: %s\r\n", "Uptime", pc_time_buffer);
		SHELL_printf("%-30s: %s (Cortex M0+)\r\n", "MCU Model Number", kpc_part_descriptors[SYS_info.k_part]);
		SHELL_printf("%-30s: %s\r\n", "Clock Source Freq", kpc_sys_clock_freq_descriptors[SYS_info.clock_source_freq]);
		SHELL_printf("%-30s: %s\r\n", "Serial Number", pc_serial_number);
		SHELL_SEPARATOR();
	}
	else
	{
		SHELL_printf("Usage: sys info\r\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

uint8_t	SYS_shell_reset(uint8_t argc, char ** argv)
{
	if (argc == 0)
	{
		SYS_LOG_DBG("System rebooting...\r\n");
		
		// Delay 10ms to empty the UART tx buffer
		CHRONOS_delay_ms(10);
		SYS_reset();
	}
	else
	{
		SHELL_printf("Usage: sys reset\r\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

uint8_t	SYS_shell_qtest(uint8_t argc, char ** argv)
{
	if (argc == 0)
	{
		// uint32_t test = 420;
		ARCADIA_msg_t msg;
		msg.id = ARCADIA_MSG_ID_NOOP;
		msg.from = ARCADIA_get_current_task_id();
		
		if (ARCADIA_send(ARCADIA_TASK_ID_DRIVE, &msg))
		{
			SYS_LOG_DBG("Message sent\r\n");
		}
		else
		{
			SYS_LOG_DBG("Failed to send message\r\n");
		}
	}
	else
	{
		SHELL_printf("Usage: sys qtest\r\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

#if ( configCHECK_FOR_STACK_OVERFLOW > 0 )
void vApplicationStackOverflowHook(TaskHandle_t xTask, char * pcTaskName )
{
	// Check pcTaskName for the name of the offending task,
	// or pxCurrentTCB if pcTaskName has itself been corrupted.
	( void ) xTask;
	( void ) pcTaskName;
}
#endif