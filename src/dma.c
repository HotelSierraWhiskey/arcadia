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
	DMA_CHANNEL_ID_DEBUG = 0,
	DMA_CHANNEL_ID_AUDIO,
	//////////
	DMA_CHANNEL_ID_NUM_IDS
} DMA_channel_id_t;

/**
 * 	An enumerated type for audio buffer IDs
 */
typedef enum _DMA_debug_buffer_id
{
	DMA_DEBUG_BUFFER_ID_ONE = 0,
	DMA_AUDIO_BUFFER_ID_TWO,
	//////////
	DMA_AUDIO_BUFFER_NUM_BUFFER_IDS
} DMA_debug_buffer_id_t;

typedef struct _DMA_debug_channel_info
{
	DMA_debug_buffer_id_t		buffer_id;
	uint8_t						u8_interrupt_config;
	uint8_t						u8_source_demo_buffer[DMA_AUDIO_BLOCK_TRANSFER_COUNT];
	uint8_t						u8_dest_demo_buffer_1[DMA_AUDIO_BLOCK_TRANSFER_COUNT];
	uint8_t						u8_dest_demo_buffer_2[DMA_AUDIO_BLOCK_TRANSFER_COUNT];
} DMA_debug_channel_info_t;

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void			DMA_channel_init				(DMA_channel_id_t channel_id);
static void			DMA_debug_channel_on_complete	(void);

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

static DMA_debug_channel_info_t debug_channel_info =
{
	.buffer_id 				= DMA_DEBUG_BUFFER_ID_ONE,
	.u8_interrupt_config 	= DMAC_CHINTENSET_TCMPL(1) | DMAC_CHINTENSET_TERR(1)
};

static dmac_descriptor_registers_t p_channel_writebacks[DMA_CHANNEL_ID_NUM_IDS];

static dmac_descriptor_registers_t p_channel_descriptors[DMA_CHANNEL_ID_NUM_IDS] =
{
	[DMA_CHANNEL_ID_DEBUG] =
	{
		.DMAC_BTCTRL = 		DMAC_BTCTRL_BEATSIZE_BYTE |
							DMAC_BTCTRL_SRCINC(1) |
							DMAC_BTCTRL_DSTINC(1) |
							DMAC_BTCTRL_VALID(1),
		.DMAC_BTCNT = 		DMA_AUDIO_BLOCK_TRANSFER_COUNT,
		.DMAC_SRCADDR =		(uint32_t)(debug_channel_info.u8_source_demo_buffer + DMA_AUDIO_BLOCK_TRANSFER_COUNT),
		.DMAC_DSTADDR = 	(uint32_t)(debug_channel_info.u8_dest_demo_buffer_1 + DMA_AUDIO_BLOCK_TRANSFER_COUNT),
		.DMAC_DESCADDR = 	DMA_NO_DESC_ADDR,
	},
	[DMA_CHANNEL_ID_AUDIO] =
	{
		.DMAC_BTCTRL = 		DMAC_BTCTRL_BEATSIZE_BYTE |
							DMAC_BTCTRL_SRCINC(1) |
							DMAC_BTCTRL_DSTINC(1) |
							DMAC_BTCTRL_VALID(1),
		.DMAC_BTCNT = 		DMA_AUDIO_BLOCK_TRANSFER_COUNT,
		// .DMAC_SRCADDR =		(uint32_t)(u8_source_demo_buffer + DMA_AUDIO_BLOCK_TRANSFER_COUNT),
		// .DMAC_DSTADDR = 	(uint32_t)(u8_dest_demo_buffer_1 + DMA_AUDIO_BLOCK_TRANSFER_COUNT),
		.DMAC_DESCADDR = 	DMA_NO_DESC_ADDR,		
	}
};

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void DMA_init(void)
{
	// Enable AHB clock for DAC. (APB is synchronous here, so just AHB required)
	MCLK_REGS->MCLK_AHBMASK |= MCLK_AHBMASK_DMAC(1);

	DMAC_REGS->DMAC_BASEADDR = (uint32_t)p_channel_descriptors;
	DMAC_REGS->DMAC_WRBADDR  = (uint32_t)p_channel_writebacks;

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
		// refactor, no switch required
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
		case DMA_CHANNEL_ID_DEBUG:
			DMAC_REGS->DMAC_CHINTENSET = debug_channel_info.u8_interrupt_config;
			break;
		case DMA_CHANNEL_ID_AUDIO:
			// DMAC_REGS->DMAC_CHINTENSET = DMA_audio_channel_config.u8_interrupt_config;
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

static void DMA_debug_channel_on_complete(void)
{
	if (debug_channel_info.buffer_id == DMA_DEBUG_BUFFER_ID_ONE)
	{
		memset(debug_channel_info.u8_dest_demo_buffer_1, 0, DMA_AUDIO_BLOCK_TRANSFER_COUNT);
		debug_channel_info.buffer_id = DMA_AUDIO_BUFFER_ID_TWO;
		p_channel_descriptors[DMA_CHANNEL_ID_DEBUG].DMAC_DSTADDR = (uint32_t)(debug_channel_info.u8_dest_demo_buffer_2 + DMA_AUDIO_BLOCK_TRANSFER_COUNT);
	}
	else
	{
		memset(debug_channel_info.u8_dest_demo_buffer_2, 0, DMA_AUDIO_BLOCK_TRANSFER_COUNT);
		p_channel_descriptors[DMA_CHANNEL_ID_DEBUG].DMAC_DSTADDR = (uint32_t)(debug_channel_info.u8_dest_demo_buffer_1 + DMA_AUDIO_BLOCK_TRANSFER_COUNT);
		debug_channel_info.buffer_id = DMA_DEBUG_BUFFER_ID_ONE;
	}

	DMAC_REGS->DMAC_CHCTRLA &= ~DMAC_CHCTRLA_ENABLE(1);
	p_channel_descriptors[DMA_CHANNEL_ID_DEBUG].DMAC_BTCTRL |= DMAC_BTCTRL_VALID(1);
	DMAC_REGS->DMAC_CHCTRLA = DMAC_CHCTRLA_ENABLE(1);
}

uint8_t DMA_shell_test(uint8_t argc, char ** argv)
{
	uint8_t b[] = {0xd, 0xe, 0xa, 0xd, 0xb, 0xe, 0xe, 0xf, 0xd, 0xe, 0xa, 0xd, 0xb, 0xe, 0xe, 0xf};

	memcpy(debug_channel_info.u8_source_demo_buffer, b, DMA_AUDIO_BLOCK_TRANSFER_COUNT);

	DMA_software_transfer(DMA_CHANNEL_ID_AUDIO);

	SHELL_printf("Buffer 1: ");
	for (uint8_t i = 0; i < DMA_AUDIO_BLOCK_TRANSFER_COUNT; i++)
	{
		SHELL_printf("%02X ", debug_channel_info.u8_dest_demo_buffer_1[i]);
	}

	SHELL_printf("\n");

	SHELL_printf("Buffer 2: ");
	for (uint8_t i = 0; i < DMA_AUDIO_BLOCK_TRANSFER_COUNT; i++)
	{
		SHELL_printf("%02X ", debug_channel_info.u8_dest_demo_buffer_2[i]);
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
					case DMA_CHANNEL_ID_DEBUG:
						DMA_debug_channel_on_complete();
						break;
					case DMA_CHANNEL_ID_AUDIO:
						// DMA_debug_channel_on_complete();
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
