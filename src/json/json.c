#include "json.h"
#include "jsmn.h"
#include "utils.h"

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	Extracts the value of the supplied key ID
 *
 * 	The supplied type ID determines how the data will be written to the supplied buffer.
 *
 * 	@param[in] 		key_id The id of the key at whose value to write
 * 	@param[in] 		type The type of the value to at which to write
 * 	@param[in] 		kpc_json The source json data
 * 	@param[out] 	p_val the data to write
 * 
 *	@return `i32_num_tokens` if the data was written, otherwise -1
 ****************************************************************************************************/
int32_t JSON_read_value(const char * kpc_key, JSON_value_type_t type, const char * kpc_json, void * p_val)
{
	ASSERT(type < JSON_VALUE_TYPE_NUM_TYPES);

	jsmn_parser 	parser;
	jsmntok_t 		p_tokens[JSON_MAX_TOKENS];
	int32_t 		i32_num_tokens;
	char 			p_val_str[JSON_MAX_KEY_SIZE];
	bool			b_res = false;

	jsmn_init(&parser);
	i32_num_tokens = jsmn_parse(&parser, kpc_json, strlen(kpc_json), p_tokens, JSON_MAX_TOKENS);

	if (i32_num_tokens > 0)
	{
		for (int32_t i = 0; i < i32_num_tokens; i++)
		{
			if (p_tokens[i].type == JSMN_STRING && 
				(strncmp(kpc_json + p_tokens[i].start, kpc_key, p_tokens[i].end - p_tokens[i].start) == 0) &&
				(strlen(kpc_key) == (size_t)(p_tokens[i].end - p_tokens[i].start)))
			{
				int32_t value_len = p_tokens[i + 1].end - p_tokens[i + 1].start;

				if (value_len < JSON_MAX_KEY_SIZE)
				{
					switch (type)
					{
						case JSON_VALUE_TYPE_INT:
						{
							strncpy(p_val_str, kpc_json + p_tokens[i + 1].start, value_len);
							p_val_str[value_len] = '\0';
							*(int32_t *)p_val = strtol(p_val_str, NULL, 10);
							b_res = true;
							break;
						}
						case JSON_VALUE_TYPE_STRING:
						{
							strncpy(p_val, kpc_json + p_tokens[i + 1].start, value_len);
							((char *)p_val)[value_len] = '\0';
							b_res = true;
							break;
						}
					}
				}
			}
		}
	}
	
	return b_res ? i32_num_tokens : -1;
}

/****************************************************************************************************
 *	Writes data to the value of a given key.
 *
 * 	The supplied type ID determines how the input data will be encoded. Ensure the type of the input
 * 	data is compatible with the given type ID.
 *
 * 	@param[in] 		key_id The id of the key at whose value to write
 * 	@param[in] 		type The type of the value to at which to write
 * 	@param[in, out] pc_json The source json data
 * 	@param[in] 		p_val the data to write
 * 
 *	@return 0 if the data was written, otherwise -1
 ****************************************************************************************************/
int32_t JSON_write_value(const char * kpc_key, JSON_value_type_t type, char * pc_json, void * p_val)
{
	ASSERT(type < JSON_VALUE_TYPE_NUM_TYPES);

	jsmn_parser 	parser;
	jsmntok_t 		p_tokens[JSON_MAX_TOKENS];
	int32_t 		i32_num_p_tokens;
	char 			pc_new_value_str[JSON_MAX_KEY_SIZE] = {0};
	int32_t 		value_start;
	int32_t 		value_end;
	int32_t 		old_value_len;
	int32_t 		new_value_len;
	bool 			b_res = false;

	jsmn_init(&parser);
	i32_num_p_tokens = jsmn_parse(&parser, pc_json, strlen(pc_json), p_tokens, JSON_MAX_TOKENS);

	if (i32_num_p_tokens > 0)
	{
		for (int32_t i = 0; i < i32_num_p_tokens; i++)
		{
			if (p_tokens[i].type == JSMN_STRING &&
				(strncmp(pc_json + p_tokens[i].start, kpc_key, p_tokens[i].end - p_tokens[i].start) == 0) &&
				(strlen(kpc_key) == (size_t)(p_tokens[i].end - p_tokens[i].start)))
			{
				switch (type)
				{
					case JSON_VALUE_TYPE_INT:
						snprintf(pc_new_value_str, JSON_MAX_KEY_SIZE, "%" PRId32, *((int32_t *)p_val));
						break;
					case JSON_VALUE_TYPE_STRING:
						snprintf(pc_new_value_str, JSON_MAX_KEY_SIZE, "\"%s\"", (char *)p_val);
						break;
				}
				
				value_start = p_tokens[i + 1].start;
				value_end = p_tokens[i + 1].end;
				old_value_len = value_end - value_start;
				new_value_len = strlen(pc_new_value_str);

				if (new_value_len <= old_value_len)
				{
					strncpy(pc_json + value_start, pc_new_value_str, new_value_len);
					if (new_value_len < old_value_len)
					{
						memmove(pc_json + value_start + new_value_len, pc_json + value_end, strlen(pc_json + value_end) + 1);
					}
					b_res = true;
					break;
				}
			}
		}
	}

	return b_res ? 0 : -1;
}
