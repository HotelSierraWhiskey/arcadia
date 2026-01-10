#include "bootloader.h"
#include "printf.h"
#include "nvmctrl.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define BOOTLOADER_IMAGE_NAME_MAX		(128U)

#define BOOTLOADER_PROGRESS_BAR_WIDTH	(25U)

typedef union _BOOTLOADER_boot_row
{
	struct
	{
		char	pc_image_name[BOOTLOADER_IMAGE_NAME_MAX];
		struct
		{
			unsigned	update : 1;
		} flags;
	} data;
	uint8_t pu8_raw[NVMCTRL_ROW_SIZE];
} BOOTLOADER_boot_row_t;

typedef struct _BOOTLOADER_info
{
	BOOTLOADER_boot_row_t	boot_row;
} BOOTLOADER_info_t;

static BOOTLOADER_info_t info;

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void 	BOOTLOADER_progress_bar_update		(const uint32_t ku32_done, const uint32_t ku32_total);
static void 	BOOTLOADER_progress_bar_finish		(void);

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void BOOTLOADER_init(void)
{
	// This is the equivalent of SYS_osc48m_init()
	/* **************************************************** */

	// Two wait states are required to run at 48MHz
	NVMCTRL_REGS->NVMCTRL_CTRLB = NVMCTRL_CTRLB_RWS_DUAL;

	// Enable in on-demand mode with a division factor of 1 (for 48MHz) with 21.33us startup delay
	OSCCTRL_REGS->OSCCTRL_OSC48MCTRL = 	OSCCTRL_OSC48MCTRL_ENABLE(1) |
										OSCCTRL_OSC48MCTRL_ONDEMAND(1);
	OSCCTRL_REGS->OSCCTRL_OSC48MDIV = 0;
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

	// This is the equivalent of  SYS_clock_init()
	/* **************************************************** */

	// Enable clock ready interrupt
	MCLK_REGS->MCLK_INTENSET = MCLK_INTENSET_CKRDY(1);

	// Configure GCLK0 with OSC48M as a clock source
	GCLK_REGS->GCLK_GENCTRL[0] = GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_OSC48M) | 
								 GCLK_GENCTRL_GENEN(1) |
								 GCLK_GENCTRL_DIVSEL(0) |
								 GCLK_GENCTRL_DIV(0) |
								 GCLK_GENCTRL_IDC(1) |
								 GCLK_GENCTRL_OE(1);

	while (GCLK_REGS->GCLK_SYNCBUSY & GCLK_SYNCBUSY_GENCTRL0(1))
	{
		continue;
	}

	// Division factor of one for the main clock. This is the reset/ default value
	MCLK_REGS->MCLK_CPUDIV = MCLK_CPUDIV_CPUDIV_DIV1;

	// Wait for the clock to be ready
	while ((MCLK_REGS->MCLK_INTFLAG & MCLK_INTFLAG_CKRDY(1)) == 0)
	{
		continue;
	}

	// Enable peripheral clock for reset controller
	MCLK_REGS->MCLK_APBAMASK |= MCLK_APBAMASK_RSTC(1);

	// This is the equivalent of IO_init()
	/* **************************************************** */
									 
	// Set the APB for the PORT peripheral
	MCLK_REGS->MCLK_APBBMASK |= MCLK_APBBMASK_PORT(1);

	// Enable printing
	PRINTF_init();

	// Init NVMCTRL module
	NVMCTRL_init();
}

void NAKED NORETURN BOOTLOADER_start_app(uint32_t u32_pc, uint32_t u32_sp, uint32_t u32_vtor)
{
	(void)u32_pc;		// r0
	(void)u32_sp;		// r1
	(void)u32_vtor;		// r2
	
	__asm__ volatile(
		"ldr r3, =0xE000ED08	\n"	// load address of SCB-VTOR register into r3
		"str r2, [r3]			\n"	// writes u32_vtor from r2 to SCB-VTOR (SCB->VTOR = u32_vtor)
		"msr msp, r1			\n"	// set MSP to u32_sp via r1
		"cpsie i				\n"	// global enable interrupts
		"bx  r0					\n"	// branch to pc (application reset handler)
	);
}

bool BOOTLOADER_update_flag_set(void)
{
	volatile uint8_t **			ppu8_rows = NVMCTRL_get_rows();
	volatile uint8_t *			pu8_row_flash = ppu8_rows[NVMCTRL_APP_NVM_ROW_ID_0];
	const uint32_t				ku32_row_addr = (uint32_t)(uintptr_t)pu8_row_flash;

	// Save the boot row data to RAM
	memcpy(&info.boot_row, (const void *)pu8_row_flash, NVMCTRL_ROW_SIZE);

	// Nuke the boot row no matter what
	NVMCTRL_erase_row(ku32_row_addr);

	if (info.boot_row.data.pc_image_name[0] == 0xFF)
	{
		return false;
	}

	info.boot_row.data.pc_image_name[BOOTLOADER_IMAGE_NAME_MAX - 1] = '\0';

	BOOT_LOG_DBG("Found image name in boot row: %s\n", info.boot_row.data.pc_image_name);

	return info.boot_row.data.flags.update ? true : false;
}

void BOOTLOADER_update_firmware(void)
{
	const uint32_t	ku32_total_steps = 1000000U;
	uint32_t		u32_step;

	for (u32_step = 0U; u32_step <= ku32_total_steps; u32_step++)
	{
		BOOTLOADER_progress_bar_update(u32_step, ku32_total_steps);
	}

	BOOTLOADER_progress_bar_finish();
}

static void BOOTLOADER_progress_bar_update(const uint32_t ku32_done, const uint32_t ku32_total)
{
	static uint32_t		u32_last_filled = 0xFFFFFFFFU;
	char				pc_bar[BOOTLOADER_PROGRESS_BAR_WIDTH + 2U + 1U]; // '[' + width + ']' + '\0'
	uint32_t			u32_total_safe;
	uint32_t			u32_done_clamped;
	uint32_t			u32_pct;
	uint32_t			u32_filled;

	u32_total_safe = (0U == ku32_total) ? 1U : ku32_total;
	u32_done_clamped = (ku32_done > u32_total_safe) ? u32_total_safe : ku32_done;

	u32_pct = (u32_done_clamped * 100U) / u32_total_safe;
	u32_filled = (u32_done_clamped * BOOTLOADER_PROGRESS_BAR_WIDTH) / u32_total_safe;

	// only redraw when we have to
	if (u32_filled == u32_last_filled)
	{
		return;
	}

	u32_last_filled = u32_filled;

	pc_bar[0] = '[';

	for (uint32_t i = 0U; i < BOOTLOADER_PROGRESS_BAR_WIDTH; i++)
	{
		pc_bar[1U + i] = (i < u32_filled) ? '#' : ' ';
	}

	pc_bar[1U + BOOTLOADER_PROGRESS_BAR_WIDTH] = ']';
	pc_bar[1U + BOOTLOADER_PROGRESS_BAR_WIDTH + 1U] = '\0';

	BOOT_LOG_DBG("%s %3lu%%", pc_bar, (unsigned long)u32_pct);
}

static void BOOTLOADER_progress_bar_finish(void)
{
	BOOTLOADER_progress_bar_update(1U, 1U);
	tfp_printf("\r\n");
}
