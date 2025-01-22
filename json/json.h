#ifndef JSON_H
#define JSON_H

#include "common.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define JSON_MAX_TOKENS 	(32U)
#define JSON_MAX_KEY_SIZE	(255U)

typedef enum _JSON_key
{
	JSON_KEY_ARCFILE_KEY_CONTENT = 0,
	JSON_KEY_ARCFILE_KEY_CHOICES,
	JSON_KEY_BOOKMARK_KEY_NODE,
	JSON_KEY_BOOKMARK_KEY_PAGE,
	//////////
	JSON_KEY_NUM_KEYS
} JSON_key_t;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

int32_t 	JSON_value_from_key		(JSON_key_t key, const char * kpc_json, uint32_t u32_json_size, char * pc_buffer);

#endif // JSON_H