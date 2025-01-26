#ifndef JSON_H
#define JSON_H

#include "common.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define JSON_MAX_TOKENS 		(32U)
#define JSON_BUFFER_MAX_SIZE	(255U)

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

int32_t 		JSON_read_value				(const char * kpc_key, JSON_value_type_t type, const char * kpc_json, void * p_val);
int32_t 		JSON_write_value			(const char * kpc_key, JSON_value_type_t type, char * pc_json, void * p_val);
bool 			JSON_get_num_objects		(const char * kpc_key, const char * kpc_json, uint8_t * pu8_num_objects);
bool 			JSON_get_key_from_index		(const char * kpc_key, const char * kpc_json, uint8_t u8_index, char * pc_output_key);
bool 			JSON_get_value_from_index	(const char * kpc_key, const char * kpc_json, uint8_t u8_index, JSON_value_type_t type, void * p_val);

#endif // JSON_H
