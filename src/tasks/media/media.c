#include "arcadia.h"
#include "utils.h"
#include "dac.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define MEDIA_LOG_DBG(fmt, ...)   		SHELL_printf("\r%-12s" fmt, "[MEDIA]", ##__VA_ARGS__)
#define MEDIA_LOG_WARN(fmt, ...)   		SHELL_PRINT_WARNING("\r%-12s" fmt, "[MEDIA]", ##__VA_ARGS__)

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void 	MEDIA_handle_message		(void);

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
		MEDIA_handle_message();
		vPortYield();
	}
}

/****************************************************************************************************
 *	MEDIA task main message handler
 *
 ****************************************************************************************************/
static void MEDIA_handle_message(void)
{
	ARCADIA_msg_t msg;

	if (ARCADIA_receive(&msg))
	{
		MEDIA_LOG_DBG("Received msg %s from %s\n", 
			ARCADIA_get_msg_type(msg.id), msg.b_sent_from_isr ? "ISR" : ARCADIA_get_task_name(msg.from));

		switch (msg.id)
		{
			case ARCADIA_MSG_ID_NOOP:
				break;

			default:
				MEDIA_LOG_DBG("Unexpected message: %u\n", msg.id);
		}
	}
}
