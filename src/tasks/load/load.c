#include "arcadia.h"
#include "utils.h"
#include "dac.h"
#include "dma.h"
#include "drive_api.h"
#include "mempool.h"
#include "tethysrt.h"
#include "load.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define LOAD_LOG_DBG(fmt, ...)   		SHELL_printf("\r%-12s" fmt, "[LOAD]", ##__VA_ARGS__)
#define LOAD_LOG_WARN(fmt, ...)   		SHELL_PRINT_WARNING("\r%-12s" fmt, "[LOAD]", ##__VA_ARGS__)

typedef enum _LOAD_symbol_id_t
{
	LOAD_SYMBOL_ID_SHELL_PRINTF = 0,
	//////////
	LOAD_SYMBOL_ID_NUM_IDS
} LOAD_symbol_id;

typedef struct _LOAD_module_slot
{
	MEMPOOL_buffer_t	ram;
	tethys_module_t		module;
} LOAD_module_slot_t;

typedef struct _LOAD_info
{
	tethys_symbol_t		p_symbols[LOAD_SYMBOL_ID_NUM_IDS];
	tethys_io_t			io;

} LOAD_info_t;

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void 	LOAD_handle_message					(void);
static void 	LOAD_handle_message_load_module		(ARCADIA_msg_t * p_msg);

int32_t			LOAD_read							(void * pv_ctx, void * pv_buf, uint32_t u32_bytes_to_read, uint32_t u32_offset);
int32_t			LOAD_get_size						(void * pv_ctx);

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

static tethys_symbol_t symbols[] =
{
	TETHYS_SYMBOL(SHELL_printf)
};

static tethys_io_t io =
{
	.read 		= LOAD_read,
	.get_size 	= LOAD_get_size
};

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	Pre-kernel module initialization function
 *
 ****************************************************************************************************/
void LOAD_init(void)
{

}

/****************************************************************************************************
 *	Top level task loop
 *
 * 	@param[in] p_params Unused
 ****************************************************************************************************/
void LOAD_task(void * p_params)
{
	UNUSED(p_params);

	MEMPOOL_buffer_t module_ram;

	module_ram = MEMPOOL_alloc(MEMPOOL_BUFFER_SIZE_ID_1K);

	file_handle_t fh = DRIVE_API_open_file(&fh, "test_1.elf", "r");

	tethys_module_t module =
	{
		.kpc_name = "test_1.elf",
		.pv_base = module_ram,
		.u32_size = MEMPOOL_BUFFER_SIZE_1024,
	};

	tethys_status_t tstat = tethys_init(symbols, 1);

	if (tstat == TETHYS_STATUS_OK)
	{
		tstat = tethys_load_module(&io, &module);

		if (tstat == TETHYS_STATUS_OK)
		{
			LOAD_LOG_DBG("load ok\n");
		}
		else
		{
			LOAD_LOG_DBG("bad load: %u\n", tstat);
		}
	}
	else
	{
		LOAD_LOG_DBG("bad init: %u\n", tstat);
	}

	module.entry(NULL);

	MEMPOOL_free(module_ram);

	while (1)
	{
		LOAD_handle_message();
		vPortYield();
	}
}

/****************************************************************************************************
 *	LOAD task main message handler
 *
 ****************************************************************************************************/
static void LOAD_handle_message(void)
{
	ARCADIA_msg_t msg;

	if (ARCADIA_receive(&msg))
	{
		LOAD_LOG_DBG("Received msg %s from %s\n", 
			ARCADIA_get_msg_type(msg.id), msg.b_sent_from_isr ? "ISR" : ARCADIA_get_task_name(msg.from));

		switch (msg.id)
		{
			case ARCADIA_MSG_ID_NOOP:
				break;

			case ARCADIA_MSG_ID_LOAD_LOAD_MODULE:
				LOAD_handle_message_load_module(&msg);
				break;

			default:
				LOAD_LOG_DBG("Unexpected message: %u\n", msg.id);
		}
	}
}

int32_t LOAD_read (void * pv_ctx, void * pv_buf, uint32_t u32_bytes_to_read, uint32_t u32_offset)
{
	int32_t i32_res = DRIVE_API_read(*((file_handle_t *)pv_ctx), (char *)pv_buf, u32_bytes_to_read, u32_offset);

	return i32_res;
}

int32_t LOAD_get_size (void * pv_ctx)
{
	return DRIVE_API_get_size(*((file_handle_t *)pv_ctx));
}

static void LOAD_handle_message_load_module(ARCADIA_msg_t * p_msg)
{
	tethys_module_t		module;
	MEMPOOL_buffer_t	module_ram = MEMPOOL_alloc(MEMPOOL_BUFFER_SIZE_ID_1K);
	tethys_status_t		tstat;

	*p_msg->payload.load_payload_load_module.p_result_status = ARCADIA_STATUS_OK;

	io.pv_ctx = &p_msg->payload.load_payload_load_module.fh;

	module.kpc_name = p_msg->payload.load_payload_load_module.pc_name,
	module.pv_base = module_ram,
	module.u32_size = MEMPOOL_BUFFER_SIZE_1024,

	tstat = tethys_load_module(&io, &module);

	if (tstat == TETHYS_STATUS_OK)
	{
		LOAD_LOG_DBG("load ok\n");
	}
	else
	{
		LOAD_LOG_DBG("bad load: %u\n", tstat);
	}

	ARCADIA_semaphore_give(p_msg->semaphore);

	LOAD_LOG_DBG("Handled msg %s with status %u\n",
		ARCADIA_get_msg_type(p_msg->id), *p_msg->payload.load_payload_load_module.p_result_status);
}
