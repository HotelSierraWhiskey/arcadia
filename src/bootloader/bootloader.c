#include "bootloader.h"
#include "printf.h"
#include "nvmctrl.h"

#define BOOTLOADER_IMAGE_NAME_MAX	(128U)

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

void BOOTLOADER_NAKED BOOTLOADER_start_app(uint32_t u32_pc, uint32_t u32_sp, uint32_t u32_vtor)
{
	(void)u32_pc;
	(void)u32_sp;
	(void)u32_vtor;
	
	__asm__ volatile(
		"ldr r3, =0xE000ED08	\n" // address of SCB-VTOR register
		"str r2, [r3]			\n"	// load SCB->VTOR into scratch register R3
		"msr msp, r1			\n"	// MSP = sp
		"cpsie i				\n"	// enable interrupts
		"bx  r0					\n"	// branch to pc
	);
}

bool BOOTLOADER_update_flag_set(void)
{
	uint8_t **					ppu8_rows = NVMCTRL_get_rows();
	volatile uint8_t *			pu8_row_flash = ppu8_rows[NVMCTRL_APP_NVM_ROW_ID_0];
	const uint32_t				ku32_row_addr = (uint32_t)(uintptr_t)pu8_row_flash;
	BOOTLOADER_boot_row_t *		p_boot_row;
	uint8_t						pu8_buf[NVMCTRL_ROW_SIZE];

	// Save the boot row data to RAM
	memcpy(pu8_buf, (const void *)pu8_row_flash, sizeof(pu8_buf));

	// Nuke the boot row no matter what
	NVMCTRL_erase_row(ku32_row_addr);

	if (pu8_buf[0] == 0xFF)
	{
		return false;
	}

	p_boot_row = (BOOTLOADER_boot_row_t *)pu8_buf;

	p_boot_row->data.pc_image_name[BOOTLOADER_IMAGE_NAME_MAX - 1] = '\0';

	BOOT_LOG_DBG("Found image name in boot row: %s\n", p_boot_row->data.pc_image_name);

	for (uint16_t i = 0; i < sizeof(pu8_buf); i++)
	{
		tfp_printf("%02X ", pu8_buf[i]);

		if ((i % 0x20) == 0x1F)
		{
			tfp_printf("\n");
		}
	}

	return false;
}
