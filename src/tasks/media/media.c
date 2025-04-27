#include "arcadia.h"
#include "utils.h"
#include "dac.h"
#include "dma.h"
#include "drive_api.h"
#include "mempool.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define MEDIA_LOG_DBG(fmt, ...)   		SHELL_printf("\r%-12s" fmt, "[MEDIA]", ##__VA_ARGS__)
#define MEDIA_LOG_WARN(fmt, ...)   		SHELL_PRINT_WARNING("\r%-12s" fmt, "[MEDIA]", ##__VA_ARGS__)

typedef enum _MEDIA_audio_state
{
	MEDIA_AUDIO_STATE_STOPPED = 0,
	MEDIA_AUDIO_STATE_PLAYING,
	//////////
	MEDIA_AUDIO_STATE_NUM_STATES
} MEDIA_audio_state_t;

typedef struct _MEDIA_audio_info
{
	file_handle_t		file;
	MEDIA_audio_state_t state;
	MEMPOOL_buffer_t	buffer_1;
	MEMPOOL_buffer_t	buffer_2;
} MEDIA_audio_info_t;

typedef struct _MEDIA_info_t
{
	MEDIA_audio_info_t audio;
} MEDIA_info_t;

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void 	MEDIA_handle_message				(void);
static void 	MEDIA_handle_message_play_audio		(ARCADIA_msg_t * p_msg);
static void 	MEDIA_handle_message_stop_audio		(ARCADIA_msg_t * p_msg);

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

static MEDIA_info_t MEDIA_info;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	Pre-kernel module initialization function
 *
 ****************************************************************************************************/
void MEDIA_init(void)
{
	// Initialize DAC driver
	DAC_init();

	// Initialize DMA channels
	DMA_init();

	MEDIA_info.audio.state = MEDIA_AUDIO_STATE_STOPPED;
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

			case ARCADIA_MSG_ID_MEDIA_PLAY_AUDIO:
				MEDIA_handle_message_play_audio(&msg);
				break;

			case ARCADIA_MSG_ID_MEDIA_STOP_AUDIO:
				MEDIA_handle_message_stop_audio(&msg);
				break;

			default:
				MEDIA_LOG_DBG("Unexpected message: %u\n", msg.id);
		}
	}
}

static void MEDIA_handle_message_play_audio(ARCADIA_msg_t * p_msg)
{
	if (MEDIA_AUDIO_STATE_PLAYING != MEDIA_info.audio.state)
	{
		if (ARCADIA_STATUS_OK == DRIVE_API_open_file(&MEDIA_info.audio.file, p_msg->payload.media_payload_play_audio.kpc_fname, "r"))
		{
			MEDIA_info.audio.buffer_1 = MEMPOOL_alloc(MEMPOOL_BUFFER_SIZE_ID_512);
			MEDIA_info.audio.buffer_2 = MEMPOOL_alloc(MEMPOOL_BUFFER_SIZE_ID_512);
			TIMER_start_dma_timer();
		}

		*p_msg->payload.media_payload_play_audio.p_result_status = ARCADIA_STATUS_OK;
	}
	else
	{
		*p_msg->payload.media_payload_play_audio.p_result_status = ARCADIA_STATUS_MEDIA_AUDIO_BUSY;
	}

	ARCADIA_semaphore_give(p_msg->semaphore);

	MEDIA_LOG_DBG("Handled msg %s with status %u\n",
				ARCADIA_get_msg_type(p_msg->id), *p_msg->payload.media_payload_play_audio.p_result_status);
}

static void MEDIA_handle_message_stop_audio(ARCADIA_msg_t * p_msg)
{
	*p_msg->payload.media_payload_play_audio.p_result_status = ARCADIA_STATUS_OK;

	if (MEDIA_AUDIO_STATE_PLAYING == MEDIA_info.audio.state)
	{
		TIMER_stop_dma_timer();
		DRIVE_API_close_file(MEDIA_info.audio.file);
		MEMPOOL_free(MEDIA_info.audio.buffer_1);
		MEMPOOL_free(MEDIA_info.audio.buffer_2);
		MEDIA_info.audio.state = MEDIA_AUDIO_STATE_STOPPED;
	}

	ARCADIA_semaphore_give(p_msg->semaphore);

	MEDIA_LOG_DBG("Handled msg %s with status %u\n",
				ARCADIA_get_msg_type(p_msg->id), *p_msg->payload.media_payload_stop_audio.p_result_status);
}

void MEDIA_update_audio_buffers(void)
{

}