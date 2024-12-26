#ifndef SPI_H
#define SPIH

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
	//////////
	SPI_CHANNEL_NUM_CHANNELS
} SPI_channel_id_t;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void		SPI_init 			(SPI_channel_id_t channel_id);
uint8_t 	SPI_exchange		(SPI_channel_id_t channel_id, uint8_t u8_byte);
void 		SPI_ss_pin_high		(SPI_channel_id_t channel_id);
void 		SPI_ss_pin_low		(SPI_channel_id_t channel_id);

/****************************************************************************************************
 *	S H E L L   F U N C T I O N S
 ****************************************************************************************************/

uint8_t		SPI_shell_write		(uint8_t argc, char ** argv);

#endif // SPI_H
