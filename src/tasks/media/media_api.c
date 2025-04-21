#include "media_api.h"
#include "media_payload.h"

ARCADIA_status_t MEDIA_API_play_audio(const char * kpc_fname)
{
	ARCADIA_status_t status = ARCADIA_STATUS_FAILED;

	MEDIA_PAYLOAD_play_audio_t payload =
	{
		.kpc_fname = kpc_fname
	};

	ARCADIA_msg_t msg =
	{
		.id = ARCADIA_MSG_ID_MEDIA_PLAY_AUDIO,
		.from = ARCADIA_get_current_task_id(),
		.payload.media_payload_play_audio = payload
	};

	msg.semaphore = ARCADIA_semaphore_alloc(&msg.semaphore_buffer);

	ARCADIA_send(ARCADIA_TASK_ID_MEDIA, &msg);

	if (!ARCADIA_semaphore_take(msg.semaphore))
	{
		status = ARCADIA_STATUS_API_TIMEOUT;
	}

	ARCADIA_semaphore_free(msg.semaphore);

	return status;
}
