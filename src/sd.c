#include "sd.h"
#include "spi.h"
#include "shell.h"
#include "chrono.h"
#include "utils.h"
#include "mempool.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define SD_LOG_DBG(fmt, ...)   			SHELL_printf("\r%-12s" fmt, "[SD]", ##__VA_ARGS__)
#define SD_LOG_WARN(fmt, ...)   		SHELL_PRINT_WARNING("\r%-12s" fmt, "[SD]", ##__VA_ARGS__)

#define SD_CMD_LEN						(6)
#define SD_RESPONSE_IDLE 				(0x01)
#define SD_RESPONSE_READY 				(0x00)
#define SD_INIT_RETRIES					(8)
#define SD_CSD_REGISTER_SIZE			(16)

/**
 *	CSD v1.0 Register
 */
typedef struct _SD_csdv1 {
  // byte 0
  unsigned _reserved_1 				: 6;
  unsigned csd_ver 					: 2;
  // byte 1
  uint8_t taac;
  // byte 2
  uint8_t nsac;
  // byte 3
  uint8_t tran_speed;
  // byte 4
  uint8_t ccc_high;
  // byte 5
  unsigned read_bl_len 				: 4;
  unsigned ccc_low 					: 4;
  // byte 6
  unsigned c_size_high 				: 2;
  unsigned _reserved_2 				: 2;
  unsigned dsr_imp 					: 1;
  unsigned read_blk_misalign 		: 1;
  unsigned write_blk_misalign 		: 1;
  unsigned read_bl_partial 			: 1;
  // byte 7
  uint8_t c_size_mid;
  // byte 8
  unsigned vdd_r_curr_max 			: 3;
  unsigned vdd_r_curr_min 			: 3;
  unsigned c_size_low 				: 2;
  // byte 9
  unsigned c_size_mult_high 		: 2;
  unsigned vdd_w_cur_max 			: 3;
  unsigned vdd_w_curr_min 			: 3;
  // byte 10
  unsigned sector_size_high 		: 6;
  unsigned erase_blk_en 			: 1;
  unsigned c_size_mult_low 			: 1;
  // byte 11
  unsigned wp_grp_size 				: 7;
  unsigned sector_size_low 			: 1;
  // byte 12
  unsigned write_bl_len_high 		: 2;
  unsigned r2w_factor 				: 3;
  unsigned _reserved_3 				: 2;
  unsigned wp_grp_enable 			: 1;
  // byte 13
  unsigned _reserved_4 				: 5;
  unsigned write_partial 			: 1;
  unsigned write_bl_len_low 		: 2;
  // byte 14
  unsigned _reserved_5				: 2;
  unsigned file_format 				: 2;
  unsigned tmp_write_protect 		: 1;
  unsigned perm_write_protect 		: 1;
  unsigned copy 					: 1;
  unsigned file_format_grp 			: 1;
  // byte 15
  unsigned always1 					: 1;
  unsigned crc 						: 7;
} PACKED SD_csdv1_t;

/**
 *	CSD v2.0 Register
 */
typedef struct _SD_csdv2 {
	// byte 0
	unsigned _reserved_1 				: 6;
	unsigned csd_ver 					: 2;
	// byte 1
	uint8_t taac;
	// byte 2
	uint8_t nsac;
	// byte 3
	uint8_t tran_speed;
	// byte 4
	uint8_t ccc_high;
	// byte 5
	unsigned read_bl_len 			: 4;
	unsigned ccc_low 				: 4;
	// byte 6
	unsigned _reserved_2 			: 4;
	unsigned dsr_imp 				: 1;
	unsigned read_blk_misalign 		: 1;
	unsigned write_blk_misalign 	: 1;
	unsigned read_bl_partial 		: 1;
	// byte 7
	unsigned _reserved_3 			: 2;
	unsigned c_size_high 			: 6;
	// byte 8
	uint8_t c_size_mid;
	// byte 9
	uint8_t c_size_low;
	// byte 10
	unsigned sector_size_high 		: 6;
	unsigned erase_blk_en 			: 1;
	unsigned _reserved_4 			: 1;
	// byte 11
	unsigned wp_grp_size 			: 7;
	unsigned sector_size_low 		: 1;
	// byte 12
	unsigned write_bl_len_high 		: 2;
	unsigned r2w_factor 			: 3;
	unsigned _reserved_5 			: 2;
	unsigned wp_grp_enable 			: 1;
	// byte 13
	unsigned _reserved_6 			: 5;
	unsigned write_partial 			: 1;
	unsigned write_bl_len_low 		: 2;
	// byte 14
	unsigned _reserved_7			: 2;
	unsigned file_format 			: 2;
	unsigned tmp_write_protect 		: 1;
	unsigned perm_write_protect 	: 1;
	unsigned copy 					: 1;
	unsigned file_format_grp 		: 1;
	// byte 15
	unsigned always1 				: 1;
	unsigned crc 					: 7;
} PACKED SD_csdv2_t;

/**
 *	CSD v3.0 Register
 */
typedef struct _SD_csdv3 {
  // Implement (p. 263)
	unsigned _;
} PACKED SD_csdv3_t;

typedef union _SD_csd
{
	SD_csdv1_t 	csdv1;
	SD_csdv2_t	csdv2;
	SD_csdv3_t	csdv3;
} SD_csd_info_t;

typedef enum _SD_csd_version
{
	SD_CSD_VERSION_STANDARD_CAPACITY = 0,		// SD
	SD_CSD_VERSION_HIGH_AND_EXTENDED_CAPACITY,	// SDHC
	SD_CSD_VERSION_ULTRA_CAPACITY,				// SDHC Ultra
	//////////
	SD_CSD_VERSION_NUM_VERSIONS
} SD_csd_version_t;

typedef struct _SD_info
{
	SD_csd_info_t		csd_info;
	bool				b_initialized;
} SD_info_t;

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static uint8_t 		SD_cmd_go_idle_state				(void);							// CMD0
static uint8_t 		SD_cmd_send_interface_condition		(void);							// CMD8
static uint8_t 		SD_cmd_app_cmd						(void);							// CMD55
static uint8_t 		SD_cmd_send_op_cond					(void);							// ACMD41
static uint8_t 		SD_cmd_read_ocr						(void); 						// CMD58
static uint8_t 		SD_cmd_set_blocklen					(uint32_t u32_blocklen); 		// CMD16
static uint8_t 		SD_cmd_write_single_block			(uint32_t u32_block_address);	// CMD24
static uint8_t 		SD_cmd_read_single_block			(uint32_t u32_block_address);	// CMD17
static uint8_t 		SD_cmd_send_csd						(uint8_t * pu8_csd);			// CMD9

static uint8_t 		SD_transfer							(uint8_t u8_byte);
static uint8_t 		SD_await_r1_response				(void);
static uint8_t 		SD_await_r7_response				(void);
static void 		SD_display_info						(void);

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

SD_info_t SD_info;

/**
 *	Descriptors for SD card types
 *
 * 	@todo
 * 	Add support for high and extended capacity and (maybe) ultra as well.
 * 	Currently, we only support standard.
 */
static const char * const kpc_version_descriptors[SD_CSD_VERSION_NUM_VERSIONS] =
{
	[SD_CSD_VERSION_STANDARD_CAPACITY]			= "SD Standard Capacity",
	[SD_CSD_VERSION_HIGH_AND_EXTENDED_CAPACITY]	= "SDHC High/ Extended Capacity",
	[SD_CSD_VERSION_ULTRA_CAPACITY]				= "SDHC Ultra Capacity",
};

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	Sd card initialization routine
 *
 * 	Initializes the card for SPI mode by transmitting 74+ clock pulses.
 *	Issues: 
 *		CMD0	(GO_IDLE_STATE)
 *		CMD8 	(SEND_IF_COND)
 *		CMD55	(APP_CMD)
 *		ACMD41 	(SD_SEND_OP_COND)
 *		CMD58	(READ_OCR)
 *		CMD16	(SET_BLOCKLEN)
 ****************************************************************************************************/
bool SD_card_init(void)
{
	uint8_t 	u8_retries = SD_INIT_RETRIES;
	uint8_t 	u8_response = 0xFF;

	SD_info.b_initialized = false;

	SPI_ss_pin_high(SPI_CHANNEL_SD_CARD);

	// send 74+ clock cycles with SS high
	for (uint8_t i = 0; i < 10; i++)
	{
		SPI_transfer(SPI_CHANNEL_SD_CARD, 0xFF);
	}

	do
	{
		u8_response = SD_cmd_go_idle_state();
	} while (u8_response != SD_RESPONSE_IDLE && --u8_retries > 0);

	if (SD_RESPONSE_IDLE == u8_response)
	{
		u8_retries = SD_INIT_RETRIES;
	}
	else
	{
		return false;
	}

	SD_cmd_send_interface_condition();

	do
	{
		CHRONO_delay_ms(250);

		u8_response =  SD_cmd_app_cmd();

		u8_response = SD_cmd_send_op_cond();

	} while (u8_response != SD_RESPONSE_READY && --u8_retries > 0);

	if (SD_RESPONSE_READY == u8_response && SD_RESPONSE_READY == SD_cmd_send_csd((uint8_t *)&SD_info.csd_info))
	{
		UNUSED(SD_cmd_read_ocr);
		UNUSED(SD_cmd_set_blocklen);

		SPI_set_baud(SPI_CHANNEL_SD_CARD, SPI_BAUD_ID_4MHZ);

		SD_info.b_initialized = true;

		SD_LOG_DBG("Initialized %s card\n", kpc_version_descriptors[SD_info.csd_info.csdv1.csd_ver]);
	}

	return SD_info.b_initialized;
}

/****************************************************************************************************
 *	Top level block write routine
 *
 * 	After transmitting a data block, the SD card sends a Data Response Token:
 * 	The response is a single byte with the following structure:
 * 
 *	0 0 0  | x x x  | 1 0 1
 *
 *	The first three bits are always 0.
 *	The next three bits (xxx) provide the status:
 * 
 *  010: Data accepted (0x05)
 *  101: Data rejected due to a CRC error
 *	110: Data rejected due to a write error
 *
 * 	@param[in] u32_block_address The address of the target block
 * 	@param[in] kpu8_buffer The data to write
 * 
 * 	@return 0 if the data was accepted, otherwise the data response token
 ****************************************************************************************************/
uint8_t SD_write_block(uint32_t u32_block_address, const uint8_t * kpu8_buffer)
{
	uint8_t u8_response = SD_cmd_write_single_block(u32_block_address);

	if (SD_RESPONSE_READY == u8_response)
	{
		SPI_ss_pin_low(SPI_CHANNEL_SD_CARD);

		// Send start token
		SD_transfer(0xFE);

		// Write the block
		for (uint16_t i = 0; i < SD_BLOCK_SIZE; i++)
		{
			SD_transfer(kpu8_buffer[i]);
		}

		SD_transfer(0xFF);
		SD_transfer(0xFF);

		u8_response = SD_transfer(0xFF);

		// Data accepted?
		if ((u8_response & 0x1F) != 0x05)
		{
			SPI_ss_pin_high(SPI_CHANNEL_SD_CARD);
			return u8_response;
		}

		// Wait for card to finish writing
		while (SD_transfer(0xFF) == 0x00)
		{
			continue;
		}

		SPI_ss_pin_high(SPI_CHANNEL_SD_CARD);
	}

	return 0;
}

/****************************************************************************************************
 *	Top level block read routine
 *
 * 	Reads data from the desired memory block
 *
 * 	@param[in] 	u32_block_address The address of the target block
 * 	@param[out] kpu8_buffer A buffer to store the read data
 * 
 * 	@return R1 response
 ****************************************************************************************************/
uint8_t SD_read_block(uint32_t u32_block_address, uint8_t * pu8_buffer)
{
	uint8_t u8_response;
	uint8_t u8_retries = 10;
	
	do
	{
		u8_response = SD_cmd_read_single_block(u32_block_address);
	} while (u8_response != SD_RESPONSE_READY && --u8_retries > 0);

	if (SD_RESPONSE_READY == u8_response)
	{
		SPI_ss_pin_low(SPI_CHANNEL_SD_CARD);

		// Wait for start token (0xFE)
		while (SD_transfer(0xFF) != 0xFE)
		{
			continue;
		}

		// Read data block
		for (uint16_t i = 0; i < SD_BLOCK_SIZE; i++)
		{
			pu8_buffer[i] = SD_transfer(0xFF);
		}

		// Read and discard CRC
		SD_transfer(0xFF);
		SD_transfer(0xFF);

		SPI_ss_pin_high(SPI_CHANNEL_SD_CARD);
	}

	return u8_response;
}

/****************************************************************************************************
 *	Returns the initialization state of the SD card
 * 
 * 	@return `true` if the card was initialized, otherwise `false`
 ****************************************************************************************************/
bool SD_is_initialized(void)
{
	return SD_info.b_initialized;
}

/****************************************************************************************************
 *	Retrieves the memory capacity of the SD card
 *
 * 	@note
 * 	The card must be initialized and its CSD data loaded into `SD_info`
 *
 * 	@return The capacity of the SD card in bytes
 ****************************************************************************************************/
uint64_t SD_get_capacity(void)
{
	// This is okay because the structure member comes first in each spec
	SD_csd_version_t 	version = SD_info.csd_info.csdv1.csd_ver;
	uint32_t 			u32_c_size;
	uint32_t			u64_capacity = 0;

	switch (version)
	{
		case SD_CSD_VERSION_STANDARD_CAPACITY:
			// Implement me
			break;

		case SD_CSD_VERSION_HIGH_AND_EXTENDED_CAPACITY:
			u32_c_size = 	((uint32_t)(SD_info.csd_info.csdv2.c_size_high & 0x3F) << 16) |
                 			((uint32_t)(SD_info.csd_info.csdv2.c_size_mid) << 8) |
                 			((uint32_t)(SD_info.csd_info.csdv2.c_size_low));

			// Calculate capacity (C_SIZE + 1) * 512KB
			u64_capacity = (u32_c_size + 1) * SD_BLOCK_SIZE * 1024;
			break;

		case SD_CSD_VERSION_ULTRA_CAPACITY:
			// Implement me
			break;
	}

	return u64_capacity;
}

/****************************************************************************************************
 *	CMD0 - GO_IDLE_STATE
 *
 * 	`0x40, 0x00, 0x00, 0x00, 0x00, 0x95`
 *
 * 	SD software reset command. Places the SD card in an idle state.
 *
 *	@return R1 response
 ****************************************************************************************************/
static uint8_t SD_cmd_go_idle_state(void)
{
	const uint8_t u8_cmd[SD_CMD_LEN] = {0x40, 0x00, 0x00, 0x00, 0x00, 0x95};

	SPI_ss_pin_low(SPI_CHANNEL_SD_CARD);

	for (uint8_t i = 0; i < SD_CMD_LEN; i++)
	{
		SD_transfer(u8_cmd[i]);
	}

	SPI_ss_pin_high(SPI_CHANNEL_SD_CARD);

	return SD_await_r1_response();
}

/****************************************************************************************************
 *	CMD8 - SEND_IF_COND
 *
 * 	`0x48, 0x00, 0x00, 0x01, 0xAA, 0x87`
 * 
 * 	Sends SD Memory Card interface condition that includes host supply voltage information and asks the
 *	accessed card whether card can operate in supplied voltage range.
 *
 *	@return R7 response
 ****************************************************************************************************/
static uint8_t SD_cmd_send_interface_condition(void)
{
	const uint8_t u8_cmd[SD_CMD_LEN] = {0x48, 0x00, 0x00, 0x01, 0xAA, 0x87};

	SPI_ss_pin_low(SPI_CHANNEL_SD_CARD);

	for (uint8_t i = 0; i < SD_CMD_LEN; i++)
	{
		SD_transfer(u8_cmd[i]);
	}

	SPI_ss_pin_high(SPI_CHANNEL_SD_CARD);

	return SD_await_r7_response();
}

/****************************************************************************************************
 *	CMD55 - APP_CMD
 *
 * 	`0x77, 0x00, 0x00, 0x00, 0x00, 0x65`
 * 
 * 	@note
 * 	Implements a default RCA (Relative Card Address of 0x00000000). This doesn't matter in
 * 	SPI mode anyway due to slave/ chip select pin.
 * 
 * 	Sends CMD55 to notify the card that the next command is application-specific (ACMD).
 *
 * 	@return R1 response
 ****************************************************************************************************/
static uint8_t SD_cmd_app_cmd(void)
{
	const uint8_t u8_cmd[SD_CMD_LEN] = {0x77, 0x00, 0x00, 0x00, 0x00, 0x65};

	SPI_ss_pin_low(SPI_CHANNEL_SD_CARD);

	for (uint8_t i = 0; i < SD_CMD_LEN; i++)
	{
		SD_transfer(u8_cmd[i]);
	}

	SPI_ss_pin_high(SPI_CHANNEL_SD_CARD);

	return SD_await_r1_response();
}

/****************************************************************************************************
 *	ACMD41 - SD_SEND_OP_COND
 *
 * 	`0x69, 0x40, 0x00, 0x00, 0x00, 0xFF`
 * 
 * 	Sends ACMD41 to initialize the SD card and check its readiness.
 *
 * 	@return R1 response (0x00 if the card is ready)
 ****************************************************************************************************/
static uint8_t SD_cmd_send_op_cond(void)
{
	const uint8_t u8_acmd[SD_CMD_LEN] = {0x69, 0x40, 0x00, 0x00, 0x00, 0xFF};

	SPI_ss_pin_low(SPI_CHANNEL_SD_CARD);

	for (uint8_t i = 0; i < SD_CMD_LEN; i++)
	{
		SD_transfer(u8_acmd[i]);
	}

	SPI_ss_pin_high(SPI_CHANNEL_SD_CARD);

	return SD_await_r1_response();
}

/****************************************************************************************************
 *	CMD58 - READ_OCR
 *
 * 	`0x7A, 0x00, 0x00, 0x00, 0x00, 0xFD`
 * 
 * 	Reads the OCR register of a card. CCS big is assigned to OCR[30]
 *
 *	@return R3 response
 ****************************************************************************************************/
static uint8_t SD_cmd_read_ocr(void)
{
	const uint8_t u8_cmd[SD_CMD_LEN] = {0x7A, 0x00, 0x00, 0x00, 0x00, 0xFD};

	SPI_ss_pin_low(SPI_CHANNEL_SD_CARD);

	for (uint8_t i = 0; i < SD_CMD_LEN; i++)
	{
		SD_transfer(u8_cmd[i]);
	}

	SPI_ss_pin_high(SPI_CHANNEL_SD_CARD);

	return SD_await_r1_response();
}

/****************************************************************************************************
 *	CMD16 - SET_BLOCKLEN
 *
 * 	Sets the block length for read/ write operations.
 * 	
 * 	@note
 * 	Not sure if this is deprecated. 512b block length is ubiquitous/ standard.
 * 	Leaving here just in case it's ever required.
 *
 * 	@param[in] u32_blocklen Block length in bytes
 * 
 * 	@return R1 response (0x00 if successful)
 ****************************************************************************************************/
static uint8_t SD_cmd_set_blocklen(uint32_t u32_blocklen)
{
	const uint8_t u8_cmd[SD_CMD_LEN] =
	{
		0x50,
		(uint8_t)(u32_blocklen >> 24),
		(uint8_t)(u32_blocklen >> 16),
		(uint8_t)(u32_blocklen >> 8),
		(uint8_t)(u32_blocklen),
		0xFF
	};

	SPI_ss_pin_low(SPI_CHANNEL_SD_CARD);

	for (uint8_t i = 0; i < SD_CMD_LEN; i++)
	{
		SD_transfer(u8_cmd[i]);
	}

	SPI_ss_pin_high(SPI_CHANNEL_SD_CARD);

	return SD_await_r1_response();
}

/****************************************************************************************************
 *	CMD24 - WRITE_SINGLE_BLOCK
 *
 * 	`0x58 <4 byte address> 0xFF`
 * 
 * 	Prepares SD card for a block write
 *
 * 	@param[in] u32_block_address The block address at which to write
 * 
 * 	@return R1 response (0x00 if successful)
 ****************************************************************************************************/
static uint8_t SD_cmd_write_single_block(uint32_t u32_block_address)
{
	// ASSERT(u32_block_address % SD_BLOCK_SIZE == 0);

	const uint8_t cmd[SD_CMD_LEN] =
	{
		0x58,
		(u32_block_address >> 24) & 0xFF,
		(u32_block_address >> 16) & 0xFF,
		(u32_block_address >> 8) & 0xFF,
		u32_block_address & 0xFF,
		0xFF
	};

	SPI_ss_pin_low(SPI_CHANNEL_SD_CARD);

	for (uint8_t i = 0; i < SD_CMD_LEN; i++)
	{
		SD_transfer(cmd[i]);
	}

	SPI_ss_pin_high(SPI_CHANNEL_SD_CARD);

	return SD_await_r1_response();
}

/****************************************************************************************************
 *	CMD17 - READ_SINGLE_BLOCK
 *
 * 	`0x51 <4 byte address> 0xFF`
 * 
 * 	Prepares SD card for a block read
 *
 * 	@param[in] u32_block_address The block address from which to read
 * 
 * 	@return R1 response (0x00 if successful)
 ****************************************************************************************************/
static uint8_t SD_cmd_read_single_block(uint32_t u32_block_address)
{
	// ASSERT(u32_block_address % SD_BLOCK_SIZE == 0);

	const uint8_t cmd[SD_CMD_LEN] =
	{
		0x51,
		(u32_block_address >> 24) & 0xFF,
		(u32_block_address >> 16) & 0xFF,
		(u32_block_address >> 8) & 0xFF,
		u32_block_address & 0xFF,
		0xFF
	};

	SPI_ss_pin_low(SPI_CHANNEL_SD_CARD);

	for (uint8_t i = 0; i < SD_CMD_LEN; i++)
	{
		SD_transfer(cmd[i]);
	}

	SPI_ss_pin_high(SPI_CHANNEL_SD_CARD);

	return SD_await_r1_response();
}

/****************************************************************************************************
 *	CMD9 - SEND_CSD
 *
 * 	`0x49, 0x00, 0x00, 0x00, 0x00, 0x95`
 * 
 * 	Reads the card's CSD register into the provided buffer
 *
 * 	@param[out] pu8_csd The buffer in which to store the CSD data
 * 
 * 	@return R1 response (0x00 if successful)
 ****************************************************************************************************/
static uint8_t SD_cmd_send_csd(uint8_t * pu8_csd)
{
	const uint8_t u8_cmd[SD_CMD_LEN] = {0x49, 0x00, 0x00, 0x00, 0x00, 0x95};

	SPI_ss_pin_low(SPI_CHANNEL_SD_CARD);

	for (uint8_t i = 0; i < SD_CMD_LEN; i++)
	{
		SD_transfer(u8_cmd[i]);
	}

	// Wait for start token (0xFE)
	while (SD_transfer(0xFF) != 0xFE)
	{
		continue;
	}

	// Read the 16-byte CSD register
	for (uint8_t i = 0; i < SD_CSD_REGISTER_SIZE; i++)
	{
		pu8_csd[i] = SD_transfer(0xFF);
	}

	// Read and discard CRC
	SD_transfer(0xFF);
	SD_transfer(0xFF);

	SPI_ss_pin_high(SPI_CHANNEL_SD_CARD);

	return 0;
}

/****************************************************************************************************
 *	Sends and receives a byte of data over the SD card's logical SPI channel.
 *
 * 	@note
 * 	The SS/ CS line must be asserted/ deasserted manually. It is not managed by hardware.
 * 
 * 	@param[in] u8_byte A byte of data to send
 *
 * 	@return A response byte
 ****************************************************************************************************/
static uint8_t SD_transfer(uint8_t u8_byte)
{
	return SPI_transfer(SPI_CHANNEL_SD_CARD, u8_byte);
}

/****************************************************************************************************
 *	Attempts to receive an R1 response from the card
 *
 * 	Will retry until failure or until an expected response is found
 *
 * 	@param[in] u8_expected The expected response
 *
 * 	@return An R1 response byte
 ****************************************************************************************************/
static uint8_t SD_await_r1_response(void)
{
	uint8_t u8_response = 0xFF;

	SPI_ss_pin_low(SPI_CHANNEL_SD_CARD);

	SD_transfer(0xFF);

	u8_response = SD_transfer(0xFF);

	SPI_ss_pin_high(SPI_CHANNEL_SD_CARD);

	return u8_response;
}

static uint8_t SD_await_r7_response(void)
{
	uint8_t u8_response = 0xFF;

	SPI_ss_pin_low(SPI_CHANNEL_SD_CARD);

	SD_transfer(0xFF);
	SD_transfer(0xFF);
	SD_transfer(0xFF);
	SD_transfer(0xFF);
	SD_transfer(0xFF);
	u8_response = SD_transfer(0xFF);

	SPI_ss_pin_high(SPI_CHANNEL_SD_CARD);

	return u8_response;
}

/****************************************************************************************************
 *	Helper function for displaying module-level information
 *
 ****************************************************************************************************/
static void SD_display_info(void)
{
	SHELL_printf("%-20s: %s\n", "Version", kpc_version_descriptors[SD_info.csd_info.csdv1.csd_ver]);
	SHELL_printf("%-20s: %llu bytes\n", "Capacity", SD_get_capacity());
}

/****************************************************************************************************
 *	S H E L L   F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	Shell utility
 *
 * 	Reads a block of memory from the SD card
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t SD_shell_read(uint8_t argc, char ** argv)
{
	uint32_t 			u32_address;
	char *				pc_char_ptr;
	MEMPOOL_buffer_t 	line_buffer = MEMPOOL_alloc(MEMPOOL_BUFFER_SIZE_ID_256);
	MEMPOOL_buffer_t 	block_buffer = MEMPOOL_alloc(MEMPOOL_BUFFER_SIZE_ID_512);

	ASSERT(line_buffer);
	ASSERT(block_buffer);

	if (argc == 1)
	{
		if (UTILS_string_to_u32(argv[0], &u32_address))
		{
			memset(block_buffer, 0, SD_BLOCK_SIZE);

			SD_read_block(u32_address, block_buffer);

			SHELL_SEPARATOR();

			for (uint16_t i = 0; i < SD_BLOCK_SIZE / 32; i++)
			{	
				pc_char_ptr = (char *)line_buffer;

				for (uint16_t j = 0; j < 32; j++)
				{
					pc_char_ptr += sprintf(pc_char_ptr, "%02X ", ((char *)block_buffer)[i * 32 + j]);
				}
				*pc_char_ptr = '\0';
				SHELL_printf("%s\n", line_buffer);
			}

			SHELL_SEPARATOR();
		}
	}
	else
	{
		SHELL_printf("Usage: sd read <addr>\n");
	}

	MEMPOOL_free(line_buffer);
	MEMPOOL_free(block_buffer);

	return SHELL_COMMAND_SUCCESS;
}

/****************************************************************************************************
 *	Shell utility
 *
 * 	Zeros out and writes a 0xC0FFEE to the addressed block of memory
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t SD_shell_wtest(uint8_t argc, char ** argv)
{
	uint32_t 			u32_address;
	MEMPOOL_buffer_t 	block_buffer = MEMPOOL_alloc(MEMPOOL_BUFFER_SIZE_ID_512);

	ASSERT(block_buffer);

	if (argc == 1)
	{
		if (UTILS_string_to_u32(argv[0], &u32_address))
		{

			memset(block_buffer, 0, SD_BLOCK_SIZE);

			// Add some 0xCOFFEE
			((char *)block_buffer)[0] = 0xC0;
			((char *)block_buffer)[1] = 0xFF;
			((char *)block_buffer)[2] = 0xEE;

			SD_write_block(u32_address, block_buffer);

			memset(block_buffer, 0, SD_BLOCK_SIZE);

			SHELL_printf("Wrote block to address 0x%08X\n", u32_address);
		}
	}
	else
	{
		SHELL_printf("Usage: sd wtest <addr>\n");
	}

	MEMPOOL_free(block_buffer);

	return SHELL_COMMAND_SUCCESS;
}

/****************************************************************************************************
 *	Shell utility
 *
 * 	Zeros out the addressed block of memory
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t SD_shell_erase(uint8_t argc, char ** argv)
{
	uint32_t 			u32_address;
	MEMPOOL_buffer_t 	block_buffer = MEMPOOL_alloc(MEMPOOL_BUFFER_SIZE_ID_512);

	ASSERT(block_buffer);

	if (argc == 1)
	{
		if (UTILS_string_to_u32(argv[0], &u32_address))
		{

			memset(block_buffer, 0, SD_BLOCK_SIZE);

			SD_write_block(u32_address, block_buffer);

			memset(block_buffer, 0, SD_BLOCK_SIZE);

			SHELL_printf("Erased block at address 0x%08X\n", u32_address);
		}
	}
	else
	{
		SHELL_printf("Usage: sd erase <addr>\n");
	}

	MEMPOOL_free(block_buffer);

	return SHELL_COMMAND_SUCCESS;
}

/****************************************************************************************************
 *	Shell utility
 *
 * 	Displays SD info
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t SD_shell_info(uint8_t argc, char ** argv)
{
	if (argc == 0)
	{
		SHELL_SEPARATOR();
		SD_display_info();
		SHELL_SEPARATOR();
	}
	else
	{
		SHELL_printf("Usage: sd info\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

/****************************************************************************************************
 *	Shell utility
 *
 * 	Displays SD info
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t SD_shell_init(uint8_t argc, char ** argv)
{
	if (argc == 0)
	{
		if (SD_card_init())
		{
			SHELL_printf("SD card initialized\n");
		}
		else
		{
			SHELL_printf("Failed to initialize SD card\n");
		}
	}
	else
	{
		SHELL_printf("Usage: sd init\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

/****************************************************************************************************
 *	Shell utility
 *
 * 	Fills the Sd card with zeros (This takes a long time)
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t SD_shell_wipe(uint8_t argc, char ** argv)
{
	MEMPOOL_buffer_t buffer = MEMPOOL_alloc(MEMPOOL_BUFFER_SIZE_ID_512);
	
	ASSERT(buffer);

	memset(buffer, 0, SD_BLOCK_SIZE);

	if (argc == 0)
	{
		if (SD_is_initialized())
		{
			SHELL_printf("This is going to take a long time\n");

			for (uint32_t i = 0; i < SD_get_capacity() / SD_BLOCK_SIZE; i++)
			{
				SD_write_block(i * SD_BLOCK_SIZE, buffer);
			}
			SHELL_printf("SD card wiped\n");
		}
		else
		{
			SHELL_printf("SD card is uninitialized\n");
		}
	}
	else
	{
		SHELL_printf("Usage: sd wipe\n");
	}

	return SHELL_COMMAND_SUCCESS;
}
