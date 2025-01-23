#include "json.h"
#include "jsmn.h"
#include "utils.h"

const char * const kpc_json_keys[JSON_KEY_NUM_KEYS] =
{
	[JSON_KEY_ARCFILE_KEY_CONTENT]	= "content",
	[JSON_KEY_ARCFILE_KEY_CHOICES]	= "choices",
	[JSON_KEY_BOOKMARK_KEY_NODE]	= "node",
	[JSON_KEY_BOOKMARK_KEY_PAGE]	= "page",
};

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

int32_t JSON_read_value(JSON_key_t key, JSON_value_type_t type, const char * kpc_json, void * p_val)
{
	ASSERT(key < JSON_KEY_NUM_KEYS);

	jsmn_parser 	parser;
	jsmntok_t 		p_tokens[JSON_MAX_TOKENS];
	int32_t 		i32_num_tokens;
	const char *	kpc_key_str = kpc_json_keys[key];
	char 			p_val_str[JSON_MAX_KEY_SIZE];
	bool			b_res = false;

	jsmn_init(&parser);
	i32_num_tokens = jsmn_parse(&parser, kpc_json, strlen(kpc_json), p_tokens, JSON_MAX_TOKENS);

	if (i32_num_tokens > 0)
	{
		for (int32_t i = 0; i < i32_num_tokens; i++)
		{
			if (p_tokens[i].type == JSMN_STRING && 
				(strncmp(kpc_json + p_tokens[i].start, kpc_key_str, p_tokens[i].end - p_tokens[i].start) == 0) &&
				(strlen(kpc_key_str) == (size_t)(p_tokens[i].end - p_tokens[i].start)))
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

int32_t JSON_write_value(JSON_key_t key, JSON_value_type_t type, char * kpc_json, void * p_val)
{
	ASSERT(key < JSON_KEY_NUM_KEYS);

	jsmn_parser 	parser;
	jsmntok_t 		p_tokens[JSON_MAX_TOKENS];
	int32_t 		i32_num_p_tokens;
	const char *	kpc_key_str = kpc_json_keys[key];
	char 			pc_new_value_str[JSON_MAX_KEY_SIZE] = {0};
	int32_t 		value_start;
	int32_t 		value_end;
	int32_t 		old_value_len;
	int32_t 		new_value_len;
	bool 			b_res = false;

	jsmn_init(&parser);
	i32_num_p_tokens = jsmn_parse(&parser, kpc_json, strlen(kpc_json), p_tokens, JSON_MAX_TOKENS);

	if (i32_num_p_tokens > 0)
	{
		for (int32_t i = 0; i < i32_num_p_tokens; i++)
		{
			if (p_tokens[i].type == JSMN_STRING &&
				(strncmp(kpc_json + p_tokens[i].start, kpc_key_str, p_tokens[i].end - p_tokens[i].start) == 0) &&
				(strlen(kpc_key_str) == (size_t)(p_tokens[i].end - p_tokens[i].start)))
			{
				switch (type)
				{
					case JSON_VALUE_TYPE_INT:
						snprintf(pc_new_value_str, JSON_MAX_KEY_SIZE, "%d", *((int32_t *)p_val));
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
					strncpy(kpc_json + value_start, pc_new_value_str, new_value_len);
					if (new_value_len < old_value_len)
					{
						memmove(kpc_json + value_start + new_value_len, kpc_json + value_end, strlen(kpc_json + value_end) + 1);
					}
					b_res = true;
					break;
				}
			}
		}
	}

	return b_res ? 0 : -1;
}
