#include "dma.h"
#include "utils.h"
#include "shell.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define DMA_AUDIO_BLOCK_TRANSFER_COUNT (16U)

typedef enum _DMA_channel_id
{
	DMA_CHANNEL_ID_AUDIO = 0,
	//////////
	DMA_CHANNEL_ID_NUM_IDS
} DMA_channel_id_t;

typedef struct _DMA_channel_config
{
	dmac_descriptor_registers_t descriptor;
	dmac_descriptor_registers_t _writeback;
} DMA_channel_config_t;

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void			DMA_channel_init		(DMA_channel_id_t channel_id);

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

uint8_t u8_source_demo_buffer[16];
uint8_t u8_dest_demo_buffer[32];


__attribute__((aligned(16))) volatile static DMA_channel_config_t DMA_channel_configs[DMA_CHANNEL_ID_NUM_IDS] =
{
	[DMA_CHANNEL_ID_AUDIO] =
	{
		.descriptor =
		{
			.DMAC_BTCTRL = 		DMAC_BTCTRL_BEATSIZE_BYTE |
								DMAC_BTCTRL_SRCINC(1) |
								DMAC_BTCTRL_DSTINC(1) |
								DMAC_BTCTRL_VALID(1),
			.DMAC_BTCNT = 		DMA_AUDIO_BLOCK_TRANSFER_COUNT,
			.DMAC_SRCADDR =		(uint32_t)(u8_source_demo_buffer + DMA_AUDIO_BLOCK_TRANSFER_COUNT),
			.DMAC_DSTADDR = 	(uint32_t)(u8_dest_demo_buffer + DMA_AUDIO_BLOCK_TRANSFER_COUNT),
			.DMAC_DESCADDR = 	0
		}
	}
};

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void DMA_init(void)
{
	// Enable AHB clock for DAC. (APB is synchronous here, so just AHB required)
	MCLK_REGS->MCLK_AHBMASK |= MCLK_AHBMASK_DMAC(1);

	DMAC_REGS->DMAC_BASEADDR = (uint32_t)&DMA_channel_configs[0].descriptor;
	DMAC_REGS->DMAC_WRBADDR  = (uint32_t)&DMA_channel_configs[0]._writeback;

	DMAC_REGS->DMAC_CHINTENSET |= DMAC_CHINTENSET_TCMPL(1);

	DMAC_REGS->DMAC_CTRL = DMAC_CTRL_DMAENABLE(1) | DMAC_CTRL_LVLEN0(1);

	for (uint8_t i = 0; i < DMA_CHANNEL_ID_NUM_IDS; i++)
	{
		DMA_channel_init(i);
	}

	NVIC_EnableIRQ(DMAC_IRQn);
}

void DMA_software_transfer(DMA_channel_id_t channel_id)
{
	ASSERT(channel_id < DMA_CHANNEL_ID_NUM_IDS);

	DMA_channel_configs[channel_id].descriptor.DMAC_BTCTRL |= DMAC_BTCTRL_VALID(1);

	SHELL_printf("\n--> DMAC_SRCADDR: %p\n", DMA_channel_configs[channel_id].descriptor.DMAC_SRCADDR);
	SHELL_printf("\n--> DMAC_DSTADDR: %p\n", DMA_channel_configs[channel_id].descriptor.DMAC_DSTADDR);

	switch (channel_id)
	{
		case DMA_CHANNEL_ID_AUDIO:
			DMAC_REGS->DMAC_SWTRIGCTRL |= DMAC_SWTRIGCTRL_SWTRIG0(1);
			break;
		default:
			// Should be unreachable
			ASSERT(0);
	}
}

static void DMA_channel_init(DMA_channel_id_t channel_id)
{
	ASSERT(channel_id < DMA_CHANNEL_ID_NUM_IDS);

	DMAC_REGS->DMAC_CHID = channel_id;

	// Configure the channel for software trigger
	DMAC_REGS->DMAC_CHCTRLB =	DMAC_CHCTRLB_TRIGACT_BLOCK |
								DMAC_CHCTRLB_TRIGSRC(0);

	DMAC_REGS->DMAC_CHCTRLA = DMAC_CHCTRLA_ENABLE(1);
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

	DMA_software_transfer(DMA_CHANNEL_ID_AUDIO);

	SHELL_printf("\n");

	for (uint8_t i = 0; i < 32; i++)
	{
		SHELL_printf("%02X ", u8_dest_demo_buffer[i]);
	}
	SHELL_printf("\n");

	return SHELL_COMMAND_SUCCESS;
}

void irqDMAC(void)
{
	for (uint8_t i = 0; i < DMA_CHANNEL_ID_NUM_IDS; i++)
	{
		if (DMAC_REGS->DMAC_INTSTATUS & (1 << i))
		{
			DMAC_REGS->DMAC_CHID = i;

			// Transfer complete
			if (DMAC_REGS->DMAC_CHINTFLAG & DMAC_CHINTFLAG_TCMPL_Msk)
			{
				DMAC_REGS->DMAC_CHINTFLAG = DMAC_CHINTFLAG_TCMPL_Msk;
				// SHELL_printf("[DMA CH %u] Transfer complete\n", ch);
			}

			// Transfer error
			if (DMAC_REGS->DMAC_CHINTFLAG & DMAC_CHINTFLAG_TERR_Msk)
			{
				DMAC_REGS->DMAC_CHINTFLAG = DMAC_CHINTFLAG_TERR_Msk;
				// SHELL_printf("[DMA CH %u] Transfer error\n", ch);
			}
		}
	}

	NVIC_ClearPendingIRQ(DMAC_IRQn);
}
