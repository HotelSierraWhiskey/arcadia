#include "dma.h"

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

typedef enum _DMA_channel_id
{
	DMA_CHANNEL_ID_AUDIO = 0,
	//////////
	DMA_CHANNEL_ID_NUM_IDS
} DMA_channel_id_t;

typedef struct _DMA_descriptor
{
	uint16_t	u16_beat_control;
	uint16_t	u16_beat_count;
	uint32_t	u32_source_address;
	uint32_t	u32_destination_address;
	uint32_t	u32_descriptor_address;
} DMA_descriptor_t;

typedef struct _DMA_channel_config
{
	DMA_descriptor_t descriptor;
	DMA_descriptor_t _writeback;
} DMA_channel_config_t;


uint8_t u8_source_demo_buffer[16];
uint8_t u8_dest_demo_buffer[32];


__attribute__((aligned(16))) static DMA_channel_config_t DMA_channel_configs[DMA_CHANNEL_ID_NUM_IDS] =
{
	[DMA_CHANNEL_ID_AUDIO] =
	{
		.descriptor =
		{
			.u16_beat_control = DMAC_BTCTRL_BEATSIZE_BYTE |
                    			DMAC_BTCTRL_SRCINC(1) |
                    			DMAC_BTCTRL_DSTINC(1) |
                    			DMAC_BTCTRL_VALID(1),
			.u16_beat_count = 			16,
			.u32_source_address =		(uint32_t)u8_source_demo_buffer + 16,
			.u32_destination_address = 	(uint32_t)u8_dest_demo_buffer,
			.u32_descriptor_address = 	0
		}
	}
};

void DMA_init(void)
{
	// Enable AHB clock for DAC
	MCLK_REGS->MCLK_AHBMASK |= MCLK_AHBMASK_DMAC(1);

	// before DMAC is enabled:
	// 		The SRAM address of where the descriptor memory section is located must be written to the Description Base Address (BASEADDR) register
	// 		The SRAM address of where the write-back section should be located must be written to the Write-Back Memory Base Address (WRBADDR) register
	DMAC_REGS->DMAC_BASEADDR = (uint32_t)&DMA_channel_configs[0].descriptor;
	DMAC_REGS->DMAC_WRBADDR  = (uint32_t)&DMA_channel_configs[0]._writeback;

	// before a DMA channel is enabled:
	// 		The channel number of the DMA channel to configure must be written to the Channel ID (CHID) register
	// 		Trigger action must be selected by writing the Trigger Action bit group in the Channel Control B register (CHCTRLB.TRIGACT)
	// 		The transfer descriptor must be made valid by writing a one to the Valid bit in the Block Transfer Control register (BTCTRL.VALID)
	//		Number of beats in the block transfer must be selected by writing the Block Transfer Count (BTCNT) register
	// 		Source address for the block transfer must be selected by writing the Block Transfer Source Address (SRCADDR) register
	// 		Destination address for the block transfer must be selected by writing the Block Transfer Destination Address (DSTADDR) register

	DMAC_REGS->DMAC_CHINTENSET |= DMAC_CHINTENSET_TCMPL(1);

	DMAC_REGS->DMAC_CTRL |= DMAC_CTRL_DMAENABLE(1);

	NVIC_EnableIRQ(DMAC_IRQn);
}

#include "shell.h"
void irqDMAC(void)
{
	SHELL_printf("Blegh\n");
	NVIC_ClearPendingIRQ(DMAC_IRQn);
}

uint8_t DMA_shell_test(uint8_t argc, char ** argv)
{
	uint8_t b[] = {0xd, 0xe, 0xa, 0xd, 0xb, 0xe, 0xe, 0xf, 0xd, 0xe, 0xa, 0xd, 0xb, 0xe, 0xe, 0xf};

	memcpy(u8_source_demo_buffer, b, 16);
	memset(u8_dest_demo_buffer, 0, 32);

	for (uint8_t i = 0; i < 32; i++)
	{
		SHELL_printf("%02X ", u8_dest_demo_buffer[i]);
	}

	DMAC_REGS->DMAC_CHID = DMA_CHANNEL_ID_AUDIO;

	// Configure the channel for software trigger
	DMAC_REGS->DMAC_CHCTRLB =
		DMAC_CHCTRLB_TRIGACT_BLOCK |
		DMAC_CHCTRLB_TRIGSRC(0);

	// Enable the channel
	DMAC_REGS->DMAC_CHCTRLA = DMAC_CHCTRLA_ENABLE(1);

	DMAC_REGS->DMAC_SWTRIGCTRL = DMAC_SWTRIGCTRL_SWTRIG0(1);

	// while (!(DMAC_REGS->DMAC_CHINTFLAG & DMAC_CHINTFLAG_TCMPL_Msk))
	// {
	// 	continue;
	// }

	DMAC_REGS->DMAC_CHINTFLAG = DMAC_CHINTFLAG_TCMPL_Msk;  // Clear flag

	SHELL_printf("\n");

	for (uint8_t i = 0; i < 32; i++)
	{
		SHELL_printf("%02X ", u8_dest_demo_buffer[i]);
	}
	SHELL_printf("\n");

	return SHELL_COMMAND_SUCCESS;
}
