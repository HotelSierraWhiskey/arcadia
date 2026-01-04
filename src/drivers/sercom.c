#include "common.h"
#include "sercom.h"

/****************************************************************************************************
 *	V A R I A B L E S
 ****************************************************************************************************/

/**
 *	GCLK ID mappings for logical SERCOM channels
 */
static const uint8_t pu8_sercom_PCHCTRL_register_indices[SERCOM_CHANNEL_ID_NUM_CHANNELS] =
{
	[SERCOM_CHANNEL_ID_0] = SERCOM0_GCLK_ID_CORE,
	[SERCOM_CHANNEL_ID_1] = SERCOM1_GCLK_ID_CORE,
	[SERCOM_CHANNEL_ID_2] = SERCOM2_GCLK_ID_CORE,
	[SERCOM_CHANNEL_ID_3] = SERCOM3_GCLK_ID_CORE
};

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	Returns the given SERCOM's peripheral channel control register index.
 *	
 *	This will allow GLCK to provide a clock signal to the given SERCOM's peripheral channel.
 *
 * 	@param[in] channel The desired SERCOM channel
 * 
 *	@return The control register index required by GCLK
 ****************************************************************************************************/
uint8_t SERCOM_get_PCHCTRL_register_index(SERCOM_channel_id_t channel)
{
	return pu8_sercom_PCHCTRL_register_indices[channel];
}
