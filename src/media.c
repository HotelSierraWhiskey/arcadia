#include "utils.h"
#include "dac.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define MEDIA_LOG_DBG(fmt, ...)   		SHELL_printf("\r%-12s" fmt, "[MEDIA]", ##__VA_ARGS__)
#define MEDIA_LOG_WARN(fmt, ...)   		SHELL_PRINT_WARNING("\r%-12s" fmt, "[MEDIA]", ##__VA_ARGS__)

/****************************************************************************************************
 *	Pre-kernel module initialization function
 *
 ****************************************************************************************************/
void MEDIA_init(void)
{
	// Initialize DAC driver
	DAC_init();
}

/****************************************************************************************************
 *	Top level task loop
 *
 * 	@param[in] p_params Unused
 ****************************************************************************************************/
void MEDIA_task(void * p_params)
{
	UNUSED(p_params);

	while (1)
	{
		// MEDIA_handle_message();
		vPortYield();
	}
}
