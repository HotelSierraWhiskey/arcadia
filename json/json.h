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

typedef enum _JSON_value_type
{
	JSON_VALUE_TYPE_INT = 0,
	JSON_VALUE_TYPE_STRING,
	//////////
	JSON_VALUE_TYPE_NUM_TYPES
} JSON_value_type_t;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

int32_t 	JSON_read_value		(JSON_key_t key, JSON_value_type_t type, const char * kpc_json, void * p_val);
int32_t 	JSON_write_value	(JSON_key_t key, JSON_value_type_t type, char * kpc_json, void * p_val);

#endif // JSON_H
