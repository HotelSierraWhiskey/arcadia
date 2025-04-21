#ifndef MEDIA_PAYLOAD_H
#define MEDIA_PAYLOAD_H

#include "common.h"
#include "arcadia_status.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

typedef struct _MEDIA_PAYLOAD_play_audio
{
	const char *			kpc_fname;
	ARCADIA_status_t *		p_result_status;
} MEDIA_PAYLOAD_play_audio_t;

typedef struct _MEDIA_PAYLOAD_stop_audio
{
	ARCADIA_status_t *		p_result_status;
} MEDIA_PAYLOAD_stop_audio_t;

#endif // MEDIA_PAYLOAD_H
