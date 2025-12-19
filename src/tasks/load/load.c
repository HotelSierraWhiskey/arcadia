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

// typedef enum _MEDIA_audio_state
// {
// 	MEDIA_AUDIO_STATE_STOPPED = 0,
// 	MEDIA_AUDIO_STATE_PLAYING,
// 	//////////
// 	MEDIA_AUDIO_STATE_NUM_STATES
// } MEDIA_audio_state_t;

// typedef struct _MEDIA_audio_info
// {
// 	file_handle_t		file;
// 	MEDIA_audio_state_t state;
// 	MEMPOOL_buffer_t	buffer_1;
// 	MEMPOOL_buffer_t	buffer_2;
// } MEDIA_audio_info_t;

// typedef struct _MEDIA_info_t
// {
// 	MEDIA_audio_info_t audio;
// } MEDIA_info_t;

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void 	LOAD_handle_message					(void);
// static void 	MEDIA_handle_message_play_audio		(ARCADIA_msg_t * p_msg);
// static void 	MEDIA_handle_message_stop_audio		(ARCADIA_msg_t * p_msg);

// static void 	MEDIA_init_audio_buffers			(void);
// static void 	MEDIA_deinit_audio_buffers			(void);

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

// static MEDIA_info_t MEDIA_info;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	Pre-kernel module initialization function
 *
 ****************************************************************************************************/
void LOAD_init(void)
{
	// Initialize DAC driver
	// DAC_init(); // temporary

	// Initialize DMA channels
	// DMA_init();

	// MEDIA_info.audio.state = MEDIA_AUDIO_STATE_STOPPED;
}

/****************************************************************************************************
 *	Top level task loop
 *
 * 	@param[in] p_params Unused
 ****************************************************************************************************/

int32_t read (void * pv_ctx, void * pv_buf, uint32_t u32_bytes_to_read, uint32_t u32_offset)
{
	int32_t i32_res = DRIVE_API_read(*((file_handle_t *)pv_ctx), (char *)pv_buf, u32_bytes_to_read, u32_offset);

	return i32_res;
}

int32_t get_size (void * pv_ctx)
{
	return DRIVE_API_get_size(*((file_handle_t *)pv_ctx));
}


void LOAD_task(void * p_params)
{
	UNUSED(p_params);

	// MEMPOOL_buffer_t module_ram;
	// tethys_symbol_t symbols[] =
	// {
	// 	TETHYS_SYMBOL(SHELL_printf)
	// };

	// module_ram = MEMPOOL_alloc(MEMPOOL_BUFFER_SIZE_ID_1K);

	// file_handle_t fh = DRIVE_API_open_file(&fh, "test_1.elf", "r");

	// tethys_io_t io =
	// {
	// 	.pv_ctx = &fh,
	// 	.read = read,
	// 	.get_size = get_size
	// };

	// tethys_module_t module =
	// {
	// 	.kpc_name = "test_1.elf",
	// 	.pv_base = module_ram,
	// 	.u32_size = MEMPOOL_BUFFER_SIZE_1024,
	// };

	// tethys_status_t tstat = tethys_init(symbols, 1);

	// if (tstat == TETHYS_STATUS_OK)
	// {
	// 	tstat = tethys_load_module(&io, &module);

	// 	if (tstat == TETHYS_STATUS_OK)
	// 	{
	// 		MEDIA_LOG_DBG("load ok\n");
	// 	}
	// 	else
	// 	{
	// 		MEDIA_LOG_DBG("bad load: %u\n", tstat);
	// 	}
	// }
	// else
	// {
	// 	MEDIA_LOG_DBG("bad init: %u\n", tstat);
	// }

	// module.entry(NULL);

	// MEMPOOL_free(module_ram);

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

			// case ARCADIA_MSG_ID_MEDIA_PLAY_AUDIO:
			// 	MEDIA_handle_message_play_audio(&msg);
			// 	break;

			// case ARCADIA_MSG_ID_MEDIA_STOP_AUDIO:
			// 	MEDIA_handle_message_stop_audio(&msg);
			// 	break;

			default:
				LOAD_LOG_DBG("Unexpected message: %u\n", msg.id);
		}
	}
}

// static void MEDIA_handle_message_play_audio(ARCADIA_msg_t * p_msg)
// {
// 	if (MEDIA_AUDIO_STATE_PLAYING != MEDIA_info.audio.state)
// 	{
// 		MEDIA_info.audio.state = MEDIA_AUDIO_STATE_PLAYING;
// 		*p_msg->payload.media_payload_play_audio.p_result_status = ARCADIA_STATUS_OK;
// 	}
// 	else
// 	{
// 		*p_msg->payload.media_payload_play_audio.p_result_status = ARCADIA_STATUS_MEDIA_AUDIO_BUSY;
// 	}

// 	MEDIA_init_audio_buffers();
// 	TIMER_start_dma_timer();

// 	ARCADIA_semaphore_give(p_msg->semaphore);

// 	MEDIA_LOG_DBG("Handled msg %s with status %u\n",
// 		ARCADIA_get_msg_type(p_msg->id), *p_msg->payload.media_payload_play_audio.p_result_status);
// }

// static void MEDIA_handle_message_stop_audio(ARCADIA_msg_t * p_msg)
// {
// 	*p_msg->payload.media_payload_play_audio.p_result_status = ARCADIA_STATUS_OK;

// 	if (MEDIA_AUDIO_STATE_PLAYING == MEDIA_info.audio.state)
// 	{
// 		TIMER_stop_dma_timer();
// 		DAC_write(0);
// 		MEDIA_deinit_audio_buffers();
// 		MEDIA_info.audio.state = MEDIA_AUDIO_STATE_STOPPED;
// 	}

// 	ARCADIA_semaphore_give(p_msg->semaphore);

// 	MEDIA_LOG_DBG("Handled msg %s with status %u\n",
// 		ARCADIA_get_msg_type(p_msg->id), *p_msg->payload.media_payload_stop_audio.p_result_status);
// }

// void MEDIA_update_audio_buffers(void)
// {

// }

// static void MEDIA_init_audio_buffers(void)
// {
// 	MEDIA_info.audio.buffer_1 = MEMPOOL_alloc(MEMPOOL_BUFFER_SIZE_ID_512);
// 	MEDIA_info.audio.buffer_2 = MEMPOOL_alloc(MEMPOOL_BUFFER_SIZE_ID_512);
// 	memset(MEDIA_info.audio.buffer_1, 0, MEMPOOL_BUFFER_SIZE_256);
// 	memset(MEDIA_info.audio.buffer_2, 0, MEMPOOL_BUFFER_SIZE_256);
// }

// static void MEDIA_deinit_audio_buffers(void)
// {
// 	MEMPOOL_free(MEDIA_info.audio.buffer_1);
// 	MEMPOOL_free(MEDIA_info.audio.buffer_2);
// }
