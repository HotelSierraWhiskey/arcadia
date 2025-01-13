#include "sys.h"
#include "io.h"
#include "shell.h"
#include "chrono.h"
#include "version.h"
#include "arcadia.h"
#include "utils.h"

#include "chrono_api.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define SYS_LOG_DBG(fmt, ...)   			SHELL_printf("\r%-12s" fmt, "[SYS]", ##__VA_ARGS__)

#define SYS_OTP5_OSC32K_CALIBRATION_MASK	(0x0007F000)

/**
 *	Manually derived calibration value
 */
#define SYS_OSC32K_CALIB_VALUE				(0x46)

/**
 *	Clock frequency enumerated type
 */
typedef enum _SYS_osc48m_freq
{
	SYS_OSC48M_FREQ_48_MHZ = 0,
	SYS_OSC48M_FREQ_24_MHZ,
	SYS_OSC48M_FREQ_16_MHZ,
	SYS_OSC48M_FREQ_12_MHZ,
	SYS_OSC48M_FREQ_9_6_MHZ,
	SYS_OSC48M_FREQ_8_MHZ,
	SYS_OSC48M_FREQ_6_86_MHZ,
	SYS_OSC48M_FREQ_6_MHZ,
	SYS_OSC48M_FREQ_5_33_MHZ,
	SYS_OSC48M_FREQ_4_8_MHZ,
	SYS_OSC48M_FREQ_4_36_MHZ,
	SYS_OSC48M_FREQ_4_MHZ,
	SYS_OSC48M_FREQ_3_69_MHZ,
	SYS_OSC48M_FREQ_3_43_MHZ,
	SYS_OSC48M_FREQ_3_2_MHZ,
	SYS_OSC48M_FREQ_3_MHZ,
	//////////
	SYS_OSC48M_FREQ_NUM_FREQ
} SYS_osc48m_freq_t;

/**
 *	Supported MCU enumerated type
 */
typedef enum _SYS_part
{
	SYS_PART_ATSAMC21E18A,
	//////////
	SYS_PART_NUM_PARTS
} SYS_part_t;

/**
 *	OSC48M settings entry
 */
typedef struct _SYS_osc48m_info
{
	const char * 	kpc_descriptor;
	uint32_t 		u32_frequency;
	uint8_t			osc_div_factor;
} SYS_osc48m_info_t;

/**
 *	Module info typedef
 */
typedef struct _SYS_info
{
	SYS_part_t			k_part;
	SYS_osc48m_info_t	osc48m_info;
} SYS_info_t;

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

/**
 *	Table of OSC48M settings
 */
static const SYS_osc48m_info_t kp_osc48m_settings[SYS_OSC48M_FREQ_NUM_FREQ] =
{
	[SYS_OSC48M_FREQ_48_MHZ] =
	{
		.kpc_descriptor = "48 MHz",
		.osc_div_factor	= OSCCTRL_OSC48MDIV_DIV_DIV1,
		.u32_frequency	= 48000000
	},
	[SYS_OSC48M_FREQ_24_MHZ] =
	{
		.kpc_descriptor = "24 MHz",
		.osc_div_factor	= OSCCTRL_OSC48MDIV_DIV_DIV2,
		.u32_frequency	= 24000000
	},
	[SYS_OSC48M_FREQ_16_MHZ] =
	{
		.kpc_descriptor = "16 MHz",
		.osc_div_factor	= OSCCTRL_OSC48MDIV_DIV_DIV3,
		.u32_frequency	= 16000000
	},
	[SYS_OSC48M_FREQ_12_MHZ] =
	{
		.kpc_descriptor = "12 MHz",
		.osc_div_factor	= OSCCTRL_OSC48MDIV_DIV_DIV4,
		.u32_frequency	= 12000000
	},
	[SYS_OSC48M_FREQ_9_6_MHZ] =
	{
		.kpc_descriptor = "9.6 MHz",
		.osc_div_factor	= OSCCTRL_OSC48MDIV_DIV_DIV5,
		.u32_frequency	= 9600000
	},
	[SYS_OSC48M_FREQ_8_MHZ] =
	{
		.kpc_descriptor = "8 MHz",
		.osc_div_factor	= OSCCTRL_OSC48MDIV_DIV_DIV6,
		.u32_frequency	= 8000000
	},
	[SYS_OSC48M_FREQ_6_86_MHZ] =
	{
		.kpc_descriptor = "6.86 MHz",
		.osc_div_factor	= OSCCTRL_OSC48MDIV_DIV_DIV7,
		.u32_frequency	= 6860000
	},
	[SYS_OSC48M_FREQ_6_MHZ] =
	{
		.kpc_descriptor = "6 MHz",
		.osc_div_factor	= OSCCTRL_OSC48MDIV_DIV_DIV8,
		.u32_frequency	= 6000000
	},
	[SYS_OSC48M_FREQ_5_33_MHZ] =
	{
		.kpc_descriptor = "5.33 MHz",
		.osc_div_factor	= OSCCTRL_OSC48MDIV_DIV_DIV9,
		.u32_frequency	= 5330000
	},
	[SYS_OSC48M_FREQ_4_8_MHZ] =
	{
		.kpc_descriptor = "4.8 MHz",
		.osc_div_factor	= OSCCTRL_OSC48MDIV_DIV_DIV10,
		.u32_frequency	= 4800000
	},
	[SYS_OSC48M_FREQ_4_36_MHZ] =
	{
		.kpc_descriptor = "4.36 MHz",
		.osc_div_factor	= OSCCTRL_OSC48MDIV_DIV_DIV11,
		.u32_frequency	= 4360000
	},
	[SYS_OSC48M_FREQ_4_MHZ] =
	{
		.kpc_descriptor = "4 MHz",
		.osc_div_factor	= OSCCTRL_OSC48MDIV_DIV_DIV12,
		.u32_frequency	= 4000000
	},
	[SYS_OSC48M_FREQ_3_69_MHZ] =
	{
		.kpc_descriptor = "3.69 MHz",
		.osc_div_factor	= OSCCTRL_OSC48MDIV_DIV_DIV13,
		.u32_frequency	= 3690000
	},
	[SYS_OSC48M_FREQ_3_43_MHZ] =
	{
		.kpc_descriptor = "3.43 MHz",
		.osc_div_factor	= OSCCTRL_OSC48MDIV_DIV_DIV14,
		.u32_frequency	= 3430000
	},
	[SYS_OSC48M_FREQ_3_2_MHZ] =
	{
		.kpc_descriptor = "3.2 MHz",
		.osc_div_factor	= OSCCTRL_OSC48MDIV_DIV_DIV15,
		.u32_frequency	= 3200000
	},
	[SYS_OSC48M_FREQ_3_MHZ] =
	{
		.kpc_descriptor = "3 MHz",
		.osc_div_factor	= OSCCTRL_OSC48MDIV_DIV_DIV16,
		.u32_frequency	= 3000000
	},
};

/**
 *	Table of MCU part descriptors
 */
static const char * const kpc_part_descriptors[SYS_PART_NUM_PARTS] = 
{
	[SYS_PART_ATSAMC21E18A] = "ATSAMC21E18A"
};

/**
 *	The sys info struct 
 */
static SYS_info_t SYS_info;

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void 	SYS_osc48m_init			(SYS_osc48m_freq_t osc48m_freq);
static void		SYS_osc32k_init			(void);
static void 	SYS_clock_init			(void);

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

	SYS_osc48m_init(SYS_OSC48M_FREQ_48_MHZ);

	SYS_osc32k_init();

	SYS_clock_init();

	IO_init();
}

/****************************************************************************************************
 *	Triggers a software reset
 *
 ****************************************************************************************************/
void SYS_reset(void)
{
	NVIC_SystemReset();
}

/****************************************************************************************************
 *	Initializes OSC48M (the internal 48MHz oscillator)
 * 
 ****************************************************************************************************/
static void SYS_osc48m_init(SYS_osc48m_freq_t osc48m_freq)
{
	SYS_info.osc48m_info = kp_osc48m_settings[osc48m_freq];

	// Two wait states are required to run at 48MHz
	NVMCTRL_REGS->NVMCTRL_CTRLB = NVMCTRL_CTRLB_RWS_DUAL;

	// Enable in on-demand mode with a division factor of 1 (for 48MHz) with 21.33us startup delay
	OSCCTRL_REGS->OSCCTRL_OSC48MCTRL = 	OSCCTRL_OSC48MCTRL_ENABLE(1) |
										OSCCTRL_OSC48MCTRL_ONDEMAND(1);
	OSCCTRL_REGS->OSCCTRL_OSC48MDIV = SYS_info.osc48m_info.osc_div_factor;
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
}

/****************************************************************************************************
 *	Initializes OSC32K (the high accuracy internal 32.768kHz oscillator)
 *
 * 	Calibration data is loaded from a one-time programmable software calibration area in NVM
 * 	before being written to OSC32K's CALIB bit group.
 * 
 * 	@warning
 * 	We need more calibration data.
 * 	The CALIB value that closest approximates 32.768kHz is ~33% less than the calibration
 * 	value in ROM. The datasheet does not specify what voltage this calibration value applies to.
 * 
 ****************************************************************************************************/
static void	SYS_osc32k_init(void)
{
	OSC32KCTRL_REGS->OSC32KCTRL_OSC32K = 	OSC32KCTRL_OSC32K_EN32K(1) |
											OSC32KCTRL_OSC32K_CALIB(SYS_OSC32K_CALIB_VALUE) |
										 	OSC32KCTRL_OSC32K_ENABLE(1);

	while ((OSC32KCTRL_REGS->OSC32KCTRL_OSC32K & OSC32KCTRL_STATUS_OSC32KRDY(1)) == 0)
	{
		continue;
	}
}

/****************************************************************************************************
 *	System Clock initialization
 *
 *	Initializes GCLK 0, which clocks the CPU.
 *	Sourced by OSC48M with a division factor of one. Enable output on GCLK_IO[0] by default.
 *
 * 	Initializes GCLK 1, which clocks the TC peripheral instances
 * 	Sourced by OSC32K with a division factor of 32768 for 1Hz. Enable output on GCLK_IO[1] by default.
 *
 * 	@note 	Signal output for the above clocks are enabled by default, but their associated peripheral
 * 			channels aren't necessarily enabled. If you need these signals for debugging, enable the
 * 			associated GCLK_IO[n] peripheral channel.
 * 
 ****************************************************************************************************/
static void SYS_clock_init(void)
{
	// Enable clock ready interrupt
	MCLK_REGS->MCLK_INTENSET = MCLK_INTENSET_CKRDY(1);

	// Configure GCLK0 with OSC48M as a clock source
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

	// Configure GCLK1 with OSC32K as a clock source
	GCLK_REGS->GCLK_GENCTRL[1] = 	GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_OSC32K) |
								 	GCLK_GENCTRL_DIV(1) |
									GCLK_GENCTRL_OE(1) |
								 	GCLK_GENCTRL_IDC(1) |
									GCLK_GENCTRL_GENEN(1);
}

/****************************************************************************************************
 *	Retrieves the configured clock source (OSC48) frequency from the 
 *	`kpu32_sys_clock_frequencies` table
 *
 *	@return The clock frequency value
 ****************************************************************************************************/
uint32_t SYS_get_source_clock_freq(void)
{
	return SYS_info.osc48m_info.u32_frequency;
}

/****************************************************************************************************
 *	S H E L L   F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	Shell utility
 *
 * 	Invokes a Hard Fault
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t SYS_shell_crash(uint8_t argc, char ** argv)
{
	if (argc == 0)
	{
		ASSERT(0);
	}
	else
	{
		SHELL_printf("Usage: sys crash\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

/****************************************************************************************************
 *	Shell utility
 *
 * 	Blocking delay in calling task's context for a given number of milliseconds
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t SYS_shell_delay(uint8_t argc, char ** argv)
{
	bool b_res = false;
	uint32_t u32_delay;

	if (argc == 1)
	{
		if (UTILS_string_to_u32(argv[0], &u32_delay))
		{
			SYS_LOG_DBG("Delaying %u ms\n", u32_delay);
			CHRONO_delay_ms(u32_delay);
			SYS_LOG_DBG("Done\n");

			b_res = true;
		}
	}

	if (!b_res)
	{
		SHELL_printf("Usage: sys delay <ms>\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

/****************************************************************************************************
 *	Shell utility
 *
 * 	Displays general system information
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t SYS_shell_info(uint8_t argc, char ** argv)
{
	// Uptime stuff
	uint32_t u32_uptime_s 	= CHRONO_get_ticks() / 1000;
	uint32_t u32_hours 		= u32_uptime_s / 3600;
	uint32_t u32_minutes 	= (u32_uptime_s % 3600) / 60;
	uint32_t u32_seconds 	= u32_uptime_s % 60;
	char pc_time_buffer[12];

	if (argc == 0)
	{
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
		SHELL_printf("%-30s: %u.%u.%u\n", "Firmware Version", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
		SHELL_printf("%-30s: %s\n", "FreeRTOS Version", VERSION_FREERTOS);
		SHELL_printf("%-30s: %s %s\n", "Compilation Timestamp", __DATE__, __TIME__);
		SHELL_printf("%-30s: %s\n", "Uptime", pc_time_buffer);
		SHELL_printf("%-30s: %s (Cortex M0+)\n", "MCU Model Number", kpc_part_descriptors[SYS_info.k_part]);
		SHELL_printf("%-30s: %s\n", "Clock Source Freq", SYS_info.osc48m_info.kpc_descriptor);
		SHELL_printf("%-30s: %s\n", "Serial Number", pc_serial_number);
		SHELL_SEPARATOR();
	}

	return SHELL_COMMAND_SUCCESS;
}

/****************************************************************************************************
 *	Shell utility
 *
 * 	Triggers a software reset
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t	SYS_shell_reset(uint8_t argc, char ** argv)
{
	if (argc == 0)
	{
		SYS_LOG_DBG("System rebooting...\n");
		
		// Delay 10ms to empty the UART tx buffer
		CHRONO_delay_ms(10);
		SYS_reset();
	}
	else
	{
		SHELL_printf("Usage: sys reset\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

/****************************************************************************************************
 *	Shell utility
 *
 * 	Retrieve high watermark for each task's stack
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t	SYS_shell_wm(uint8_t argc, char ** argv)
{
	TaskHandle_t 	handle;
	uint16_t 		u16_used_stack_space;
	uint32_t		u32_total_stack_size;

	if (argc == 0)
	{
		SHELL_SEPARATOR();
		SHELL_printf("Stack High Watermarks\n");
		SHELL_SEPARATOR();
		for (uint8_t i = 0; i < ARCADIA_TASK_ID_NUM_IDS; i++)
		{
			handle = ARCADIA_handle_from_id(i);

			ASSERT(handle);

			u32_total_stack_size = ARCADIA_get_task_stack_size_words(i) * 4;
			u16_used_stack_space = u32_total_stack_size - (uxTaskGetStackHighWaterMark(handle) * 4);

			SHELL_printf("%-10s %u bytes of %u available (%.2f%%)\n",
				ARCADIA_get_task_name(i),
				u16_used_stack_space,
				u32_total_stack_size,
				((float)u16_used_stack_space / (float)(u32_total_stack_size)) * 100.0F);
				CHRONO_delay_ms(5);
		}
		SHELL_SEPARATOR();

	}
	else
	{
		SHELL_printf("Usage: sys wm\n");
	}

	return SHELL_COMMAND_SUCCESS;
}


#if ( configCHECK_FOR_STACK_OVERFLOW > 0 )
/****************************************************************************************************
 *	Stack overflow hook
 *
 ****************************************************************************************************/
void vApplicationStackOverflowHook(TaskHandle_t xTask, char * pcTaskName )
{
	// Check pcTaskName for the name of the offending task,
	// or pxCurrentTCB if pcTaskName has itself been corrupted.
	( void ) xTask;
	( void ) pcTaskName;
}
#endif
