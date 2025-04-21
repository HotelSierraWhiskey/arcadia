#include "dma.h"
#include "utils.h"
#include "shell.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define DMA_NO_DESC_ADDR				(0U)
#define DMA_AUDIO_BLOCK_TRANSFER_COUNT 	(4U)

/**
 * 	DMA logical channel IDs
 */
typedef enum _DMA_channel_id
{
	DMA_CHANNEL_ID_AUDIO = 0,
	//////////
	DMA_CHANNEL_ID_NUM_IDS
} DMA_channel_id_t;

/**
 * 	An enumerated type for audio buffer IDs
 */
typedef enum _DMA_audio_buffer_id
{
	DMA_AUDIO_BUFFER_ID_ONE = 0,
	DMA_AUDIO_BUFFER_ID_TWO,
	//////////
	DMA_AUDIO_BUFFER_NUM_BUFFER_IDS
} DMA_audio_buffer_id_t;

/**
 * 	Config structure for the audio DMA channel
 */
typedef struct _DMA_audio_channel_config
{
	DMA_audio_buffer_id_t		buffer_id;
	uint8_t						u8_interrupt_config;
	dmac_descriptor_registers_t descriptor;
	dmac_descriptor_registers_t _writeback;
} DMA_audio_channel_config_t;

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void			DMA_channel_init				(DMA_channel_id_t channel_id);
static void			DMA_audio_channel_on_complete	(void);

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

uint8_t u8_source_demo_buffer[DMA_AUDIO_BLOCK_TRANSFER_COUNT] = { 0 };
uint8_t u8_dest_demo_buffer_1[DMA_AUDIO_BLOCK_TRANSFER_COUNT] = { 0 };
uint8_t u8_dest_demo_buffer_2[DMA_AUDIO_BLOCK_TRANSFER_COUNT] = { 0 };

static volatile DMA_audio_channel_config_t DMA_audio_channel_config =
{
	.buffer_id = 			DMA_AUDIO_BUFFER_ID_ONE,
	.u8_interrupt_config = 	DMAC_CHINTENSET_TCMPL(1) | DMAC_CHINTENSET_TERR(1),
	.descriptor =
	{
		.DMAC_BTCTRL = 		DMAC_BTCTRL_BEATSIZE_BYTE |
							DMAC_BTCTRL_SRCINC(1) |
							DMAC_BTCTRL_DSTINC(1) |
							DMAC_BTCTRL_VALID(1),
		.DMAC_BTCNT = 		DMA_AUDIO_BLOCK_TRANSFER_COUNT,
		.DMAC_SRCADDR =		(uint32_t)(u8_source_demo_buffer + DMA_AUDIO_BLOCK_TRANSFER_COUNT),
		.DMAC_DSTADDR = 	(uint32_t)(u8_dest_demo_buffer_1 + DMA_AUDIO_BLOCK_TRANSFER_COUNT),
		.DMAC_DESCADDR = 	DMA_NO_DESC_ADDR,
	},
};

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void DMA_init(void)
{
	// Enable AHB clock for DAC. (APB is synchronous here, so just AHB required)
	MCLK_REGS->MCLK_AHBMASK |= MCLK_AHBMASK_DMAC(1);

	DMAC_REGS->DMAC_BASEADDR = (uint32_t)&DMA_audio_channel_config.descriptor;
	DMAC_REGS->DMAC_WRBADDR  = (uint32_t)&DMA_audio_channel_config._writeback;

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

	DMAC_REGS->DMAC_CHID = channel_id;

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

	switch (channel_id)
	{
		case DMA_CHANNEL_ID_AUDIO:
			DMAC_REGS->DMAC_CHINTENSET = DMA_audio_channel_config.u8_interrupt_config;
			break;
		default:
			// Should be unreachable
			ASSERT(0);
	}

	// Configure the channel for software trigger (temporary... should probably have a config for this)
	DMAC_REGS->DMAC_CHCTRLB =	DMAC_CHCTRLB_TRIGACT_BEAT |
								DMAC_CHCTRLB_TRIGSRC(0);


	DMAC_REGS->DMAC_CHCTRLA = DMAC_CHCTRLA_ENABLE(1);
}

static void DMA_audio_channel_on_complete(void)
{
	if (DMA_audio_channel_config.buffer_id == DMA_AUDIO_BUFFER_ID_ONE)
	{
		memset(u8_dest_demo_buffer_1, 0, DMA_AUDIO_BLOCK_TRANSFER_COUNT);
		DMA_audio_channel_config.buffer_id = DMA_AUDIO_BUFFER_ID_TWO;
		DMA_audio_channel_config.descriptor.DMAC_DSTADDR = (uint32_t)(u8_dest_demo_buffer_2 + DMA_AUDIO_BLOCK_TRANSFER_COUNT);
	}
	else
	{
		memset(u8_dest_demo_buffer_2, 0, DMA_AUDIO_BLOCK_TRANSFER_COUNT);
		DMA_audio_channel_config.buffer_id = DMA_AUDIO_BUFFER_ID_ONE;
		DMA_audio_channel_config.descriptor.DMAC_DSTADDR = (uint32_t)(u8_dest_demo_buffer_1 + DMA_AUDIO_BLOCK_TRANSFER_COUNT);
	}

	DMAC_REGS->DMAC_CHCTRLA &= ~DMAC_CHCTRLA_ENABLE(1);
	DMA_audio_channel_config.descriptor.DMAC_BTCTRL |= DMAC_BTCTRL_VALID(1);
	DMAC_REGS->DMAC_CHCTRLA = DMAC_CHCTRLA_ENABLE(1);
}

uint8_t DMA_shell_test(uint8_t argc, char ** argv)
{
	uint8_t b[] = {0xd, 0xe, 0xa, 0xd, 0xb, 0xe, 0xe, 0xf, 0xd, 0xe, 0xa, 0xd, 0xb, 0xe, 0xe, 0xf};

	memcpy(u8_source_demo_buffer, b, DMA_AUDIO_BLOCK_TRANSFER_COUNT);

	DMA_software_transfer(DMA_CHANNEL_ID_AUDIO);

	SHELL_printf("Buffer 1: ");
	for (uint8_t i = 0; i < DMA_AUDIO_BLOCK_TRANSFER_COUNT; i++)
	{
		SHELL_printf("%02X ", u8_dest_demo_buffer_1[i]);
	}

	SHELL_printf("\n");

	SHELL_printf("Buffer 2: ");
	for (uint8_t i = 0; i < DMA_AUDIO_BLOCK_TRANSFER_COUNT; i++)
	{
		SHELL_printf("%02X ", u8_dest_demo_buffer_2[i]);
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

				switch (i)
				{
					case DMA_CHANNEL_ID_AUDIO:
						DMA_audio_channel_on_complete();
						break;
					default:
						ASSERT(0);
				}
				SHELL_printf("Complete\n");
			}

			// Transfer error (TODO: this should be handled properly)
			if (DMAC_REGS->DMAC_CHINTFLAG & DMAC_CHINTFLAG_TERR_Msk)
			{
				DMAC_REGS->DMAC_CHINTFLAG = DMAC_CHINTFLAG_TERR_Msk;
				SHELL_printf("Error\n");

			}
		}
	}

	NVIC_ClearPendingIRQ(DMAC_IRQn);
}
