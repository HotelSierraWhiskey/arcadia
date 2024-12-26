#include "sd.h"
#include "spi.h"
#include "shell.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define SD_LOG_DBG(fmt, ...)   			SHELL_printf("%-10s" fmt, "[SD]", ##__VA_ARGS__)
#define SD_LOG_WARN(fmt, ...)   		SHELL_PRINT_WARNING("%-10s" fmt, "[SD]", ##__VA_ARGS__)

#define SD_CMD_LEN						(6)
#define SD_DEFAULT_BLOCKLEN 			(512U)
#define SD_RESPONSE_IDLE 				(0x01)
#define SD_RESPONSE_READY 				(0x00)

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

/**
 *	SD CMDs	
 */
static uint8_t 		SD_cmd_go_idle_state				(void);						// CMD0
static uint8_t 		SD_cmd_send_interface_condition		(void);						// CMD8
static uint8_t 		SD_cmd_app_cmd						(void);						// CMD55
static uint8_t 		SD_cmd_send_op_cond					(void);						// ACMD41
static uint8_t 		SD_cmd_read_ocr						(void); 					// CMD58
static uint8_t 		SD_cmd_set_blocklen					(uint32_t u32_blocklen); 	// CMD16

/**
 *	SD utilities
 */
static uint8_t 		SD_exchange							(uint8_t u8_byte);
static uint8_t 		SD_await_r1_response				(uint8_t u8_expected);

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	Sd card initialization routine
 *
 * 	Initializes the card for SPI mode by transmitting 74+ clock pulses.
 *	Issues: 
 *		CMD0	(GO_IDLE_STATE),
 *		CMD8 	(SEND_IF_COND),
 *		CMD55	(APP_CMD)
 *		ACMD41 	(SD_SEND_OP_COND)
 *		CMD58	(READ_OCR),
 *		CMD16	(SET_BLOCKLEN)
 ****************************************************************************************************/
void SD_card_init(void)
{
	uint8_t u8_response;

	SPI_ss_pin_high(SPI_CHANNEL_SD_CARD);

	// Send 74+ clock cycles with SS high
	for (uint8_t i = 0; i < 10; i++)
	{
		SPI_exchange(SPI_CHANNEL_SD_CARD, 0xFF);
	}

	u8_response = SD_cmd_go_idle_state();
	SD_LOG_DBG("SD_cmd_go_idle_state\r\n");

	u8_response = SD_cmd_send_interface_condition();
	SD_LOG_DBG("SD_cmd_send_interface_condition\r\n");

	do
	{
		u8_response = SD_cmd_app_cmd();
		SD_LOG_DBG("SD_cmd_app_cmd\r\n");

		u8_response = SD_cmd_send_op_cond();
		SD_LOG_DBG("SD_cmd_send_op_cond\r\n");

	} while (SD_RESPONSE_IDLE == u8_response);

	u8_response = SD_cmd_read_ocr();
	SD_LOG_DBG("SD_cmd_read_ocr\r\n");

	u8_response = SD_cmd_set_blocklen(SD_DEFAULT_BLOCKLEN);
	SD_LOG_DBG("SD_cmd_set_blocklen\r\n");
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
		SD_exchange(u8_cmd[i]);
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
		SD_exchange(u8_cmd[i]);
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
		SD_exchange(u8_cmd[i]);
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
		SD_exchange(u8_acmd[i]);
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
		SD_exchange(u8_cmd[i]);
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
		0x50,                         		// CMD16 identifier
		(uint8_t)(u32_blocklen >> 24),		// MSB of block length
		(uint8_t)(u32_blocklen >> 16),		// Next byte
		(uint8_t)(u32_blocklen >> 8), 		// Next byte
		(uint8_t)(u32_blocklen),      		// LSB of block length
		0xFF                          		// Dummy CRC
	};

	SPI_ss_pin_low(SPI_CHANNEL_SD_CARD);

	for (uint8_t i = 0; i < SD_CMD_LEN; i++)
	{
		SD_exchange(u8_cmd[i]);
	}

	SPI_ss_pin_high(SPI_CHANNEL_SD_CARD);

	return SD_await_r1_response(SD_RESPONSE_READY);
}


static uint8_t SD_exchange(uint8_t u8_byte)
{
	return SPI_exchange(SPI_CHANNEL_SD_CARD, u8_byte);
}








static uint8_t SD_await_r1_response(uint8_t u8_expected)
{
	uint8_t u8_response = 0xFF;

	SPI_ss_pin_low(SPI_CHANNEL_SD_CARD);

	for (uint8_t i = 0; i < 10; i++)
	{
		if (u8_expected == SPI_exchange(SPI_CHANNEL_SD_CARD, 0xFF))
		{
			u8_response = u8_expected;
		}
	}

	SPI_ss_pin_high(SPI_CHANNEL_SD_CARD);

	return u8_response;
}
