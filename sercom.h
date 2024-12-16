#ifndef SERCOM_H
#define SERCOM_H

#include "common.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

/**
 *	An enumerated type for SERCOM channels
 */
typedef enum SERCOM_channel_id
{
	SERCOM_CHANNEL_ID_0 = 0,
	SERCOM_CHANNEL_ID_1,
	SERCOM_CHANNEL_ID_2,
	SERCOM_CHANNEL_ID_3,
	//////////
	SERCOM_CHANNEL_ID_NUM_CHANNELS
} SERCOM_channel_id_t;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

uint8_t		SERCOM_get_PCHCTRL_register_index	(SERCOM_channel_id_t channel);

#endif // SERCOM_H
