#include "common.h"
#include "sercom.h"

/****************************************************************************************************
 *	V A R I A B L E S
 ****************************************************************************************************/

static const uint8_t pu8_sercom_PCHCTRL_register_indices[SERCOM_CHANNEL_NUM_CHANNELS] =
{
	[SERCOM_CHANNEL_0] = 19,
	[SERCOM_CHANNEL_1] = 20,
	[SERCOM_CHANNEL_2] = 21,
	[SERCOM_CHANNEL_3] = 22
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
uint8_t SERCOM_get_PCHCTRL_register_index(SERCOM_channel_t channel)
{
	return pu8_sercom_PCHCTRL_register_indices[channel];
}
