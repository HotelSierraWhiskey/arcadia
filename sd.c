#include "sd.h"
#include "spi.h"
#include "shell.h"
#include "chrono.h"
#include "utils.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define SD_LOG_DBG(fmt, ...)   			SHELL_printf("%-10s" fmt, "[SD]", ##__VA_ARGS__)
#define SD_LOG_WARN(fmt, ...)   		SHELL_PRINT_WARNING("%-10s" fmt, "[SD]", ##__VA_ARGS__)

#define SD_CMD_LEN						(6)
#define SD_BLOCK_SIZE 					(512U)
#define SD_RESPONSE_IDLE 				(0x01)
#define SD_RESPONSE_READY 				(0x00)
#define SD_INIT_RETRIES					(5)

typedef struct _SD_info
{
	uint8_t	pu8_buffer[SD_BLOCK_SIZE];
} SD_info_t;

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

/**
 *	SD CMDs
 */
static uint8_t 		SD_cmd_go_idle_state				(void);							// CMD0
static uint8_t 		SD_cmd_send_interface_condition		(void);							// CMD8
static uint8_t 		SD_cmd_app_cmd						(void);							// CMD55
static uint8_t 		SD_cmd_send_op_cond					(void);							// ACMD41
static uint8_t 		SD_cmd_read_ocr						(void); 						// CMD58
static uint8_t 		SD_cmd_set_blocklen					(uint32_t u32_blocklen); 		// CMD16
static uint8_t 		SD_cmd_write_single_block			(uint32_t u32_block_address);	// CMD24
static uint8_t 		SD_cmd_read_single_block			(uint32_t u32_block_address);	// CMD17

/**
 *	SD utilities
 */
static uint8_t 		SD_transfer							(uint8_t u8_byte);
static uint8_t 		SD_await_r1_response				(uint8_t u8_expected);

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

SD_info_t SD_info;

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
void SD_card_init(void)
{
	uint8_t u8_retries = SD_INIT_RETRIES;
	uint8_t u8_response;

	memset(SD_info.pu8_buffer, 0 , SD_BLOCK_SIZE);

	SPI_ss_pin_high(SPI_CHANNEL_SD_CARD);

	// send 74+ clock cycles with SS high
	for (uint8_t i = 0; i < 10; i++)
	{
		SPI_transfer(SPI_CHANNEL_SD_CARD, 0xFF);
	}

	u8_response = SD_cmd_go_idle_state();
	SD_LOG_DBG("SD_cmd_go_idle_state: %u\r\n", u8_response);

	u8_response = SD_cmd_send_interface_condition();
	SD_LOG_DBG("SD_cmd_send_interface_condition: %u\r\n", u8_response);

	do
	{
		CHRONO_delay_ms(500);

		u8_response = SD_cmd_app_cmd();
		SD_LOG_DBG("SD_cmd_app_cmd: %u\r\n", u8_response);

		u8_response = SD_cmd_send_op_cond();
		SD_LOG_DBG("SD_cmd_send_op_cond: %u\r\n", u8_response);

	} while (u8_response != SD_RESPONSE_READY && --u8_retries > 0);

	u8_response = SD_cmd_read_ocr();
	SD_LOG_DBG("SD_cmd_read_ocr %u\r\n", u8_response);

	u8_response = SD_cmd_set_blocklen(SD_BLOCK_SIZE);
	SD_LOG_DBG("SD_cmd_set_blocklen %u\r\n", u8_response);

	// SPI_set_baud(SPI_CHANNEL_SD_CARD, SPI_BAUD_ID_25MHZ);
}

uint8_t SD_write_block(uint32_t u32_block_address, const uint8_t * kpu8_buffer)
{
	uint8_t u8_response = SD_cmd_write_single_block(u32_block_address);
	SD_LOG_DBG("SD_cmd_write_single_block %u\r\n", u8_response);

	SPI_ss_pin_low(SPI_CHANNEL_SD_CARD);

	// Send start token
    SD_transfer(0xFE);

	// Write the block
	for (uint16_t i = 0; i < SD_BLOCK_SIZE; i++)
    {
        SD_transfer(kpu8_buffer[i]);
    }

	// Dummy CRC
    SD_transfer(0xFF);
    SD_transfer(0xFF);

	// Check data response token
    u8_response = SD_transfer(0xFF);

    if ((u8_response & 0x1F) != 0x05) // Data accepted
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

    return 0;
}

uint8_t SD_read_block(uint32_t u32_block_address, uint8_t * pu8_buffer)
{
	uint8_t u8_response = SD_cmd_read_single_block(u32_block_address);

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

    return 0;
}

/****************************************************************************************************
 *	CMD0 - GO_IDLE_STATE
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

	return SD_await_r1_response(SD_RESPONSE_IDLE);
}

/****************************************************************************************************
 *	CMD8 - SEND_IF_COND
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

	return SD_await_r1_response(SD_RESPONSE_IDLE);
}

/****************************************************************************************************
 *	CMD55 - APP_CMD
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

	return SD_await_r1_response(SD_RESPONSE_IDLE);
}

/****************************************************************************************************
 *	ACMD41 - SD_SEND_OP_COND
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

	return SD_await_r1_response(SD_RESPONSE_READY);
}

/****************************************************************************************************
 *	CMD58 - READ_OCR
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

	return SD_await_r1_response(SD_RESPONSE_READY);
}

/****************************************************************************************************
 *	CMD16 - SET_BLOCKLEN
 *
 * 	Sets the block length for read/ write operations.
 *
 * 	@param[in] u32_blocklen Block length in bytes (e.g., 512 for standard SD cards).
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

	return SD_await_r1_response(SD_RESPONSE_READY);
}

static uint8_t SD_cmd_write_single_block(uint32_t u32_block_address)
{
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

	return SD_await_r1_response(SD_RESPONSE_READY);
}

static uint8_t SD_cmd_read_single_block(uint32_t u32_block_address)
{
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

	return SD_await_r1_response(SD_RESPONSE_READY);
}

static uint8_t SD_transfer(uint8_t u8_byte)
{
	return SPI_transfer(SPI_CHANNEL_SD_CARD, u8_byte);
}

static uint8_t SD_await_r1_response(uint8_t u8_expected)
{
	uint8_t u8_response = 0xFF;

	SPI_ss_pin_low(SPI_CHANNEL_SD_CARD);

	for (uint8_t i = 0; i < 10; i++)
	{
		if (u8_expected == SD_transfer(0xFF))
		{
			u8_response = u8_expected;
			break;
		}
	}

	SPI_ss_pin_high(SPI_CHANNEL_SD_CARD);

	return u8_response;
}

/****************************************************************************************************
 *	S H E L L   F U N C T I O N S
 ****************************************************************************************************/

uint8_t SD_shell_read(uint8_t argc, char ** argv)
{
	uint32_t 	u32_address;

	if (argc == 1)
	{
		if (UTILS_string_to_u32(argv[0], &u32_address))
		{
			memset(SD_info.pu8_buffer, 0, SD_BLOCK_SIZE);

			SD_read_block(u32_address, SD_info.pu8_buffer);

			SHELL_SEPARATOR();

			for (uint16_t i = 0; i < SD_BLOCK_SIZE / 32; i++)
			{	
				for (uint16_t j = 0; j < 32; j++)
				{
					SHELL_printf("%02X ", SD_info.pu8_buffer[i * 32 + j]);
					CHRONO_delay_ms(1); // Fixme
				}
				SHELL_printf("\r\n");
			}

			SHELL_SEPARATOR();
		}
	}
	else
	{
		SHELL_printf("Usage: sd read <addr>\r\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

uint8_t SD_shell_wtest(uint8_t argc, char ** argv)
{
	uint32_t u32_address;

	if (argc == 1)
	{
		if (UTILS_string_to_u32(argv[0], &u32_address))
		{
			// dummy data 0 - 255 twice, as blocksizes are 512 bytes
			for (uint16_t i = 0; i < SD_BLOCK_SIZE; i++)
			{
				SD_info.pu8_buffer[i] = i & 0xFF;
			}

			SD_info.pu8_buffer[0] = 0xC0;
			SD_info.pu8_buffer[1] = 0xFF;
			SD_info.pu8_buffer[2] = 0xEE;

			SD_write_block(u32_address, SD_info.pu8_buffer);

			memset(SD_info.pu8_buffer, 0, SD_BLOCK_SIZE);

			SHELL_printf("Wrote block to address 0x%08X\r\n", u32_address);
		}
	}
	else
	{
		SHELL_printf("Usage: sd wtest <addr>\r\n");
	}

	return SHELL_COMMAND_SUCCESS;
}
