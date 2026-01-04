#ifndef SPI_H
#define SPI_H

#include "common.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

/**
 *	Logical SPI channel IDs
 */
typedef enum _SPI_channel_id
{
	SPI_CHANNEL_SD_CARD = 0,
	SPI_CHANNEL_DISPLAY,
	//////////
	SPI_CHANNEL_NUM_CHANNELS
} SPI_channel_id_t;

/**
 *	Baud rate IDs
 */
typedef enum _SPI_baud_id
{
	SPI_BAUD_ID_400KHZ = 0,
	SPI_BAUD_ID_4MHZ,
	SPI_BAUD_ID_25MHZ,
	//////////
	SPI_BAUD_ID_NUM_IDS,
} SPI_baud_id_t;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void		SPI_init 			(SPI_channel_id_t channel_id);
void		SPI_set_baud 		(SPI_channel_id_t channel_id, SPI_baud_id_t baud_id);
uint8_t 	SPI_transfer		(SPI_channel_id_t channel_id, uint8_t u8_byte);
void 		SPI_ss_pin_high		(SPI_channel_id_t channel_id);
void 		SPI_ss_pin_low		(SPI_channel_id_t channel_id);

/****************************************************************************************************
 *	S H E L L   F U N C T I O N S
 ****************************************************************************************************/

uint8_t		SPI_shell_write		(uint8_t argc, char ** argv);
uint8_t		SPI_shell_info		(uint8_t argc, char ** argv);

#endif // SPI_H
