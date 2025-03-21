#ifndef PV035HV_CINA5007
#define PV035HV_CINA5007

#include "common.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

typedef enum _PV035HV_CINA5007_mode
{
	PV035HV_CINA5007_MODE_SPI = 0,
	//////////
	PV035HV_CINA5007_MODE_NUM_MODES
} PV035HV_CINA5007_mode_t;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void        PV035HV_CINA5007_init       	(PV035HV_CINA5007_mode_t mode);
void 		PV035HV_CINA5007_write_command	(uint8_t u8_cmd);
void 		PV035HV_CINA5007_write_data		(uint8_t u8_data);

#endif // PV035HV_CINA5007
