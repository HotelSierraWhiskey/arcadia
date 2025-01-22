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

int32_t JSON_value_from_key(JSON_key_t key, const char * kpc_json, uint32_t u32_json_size, char * pc_buffer)
{
	ASSERT(key < JSON_KEY_NUM_KEYS);

	jsmn_parser 	parser;
	jsmntok_t 		tokens[JSON_MAX_TOKENS];
	int32_t 		i32_num_tokens;
	const char *	key_str = kpc_json_keys[key];
	bool			b_res = false;

	jsmn_init(&parser);
	i32_num_tokens = jsmn_parse(&parser, kpc_json, u32_json_size, tokens, JSON_MAX_TOKENS);

	if (i32_num_tokens > 0)
	{
		for (int32_t i = 0; i < i32_num_tokens; i++)
		{
			if (tokens[i].type == JSMN_STRING && 
				(strncmp(kpc_json + tokens[i].start, key_str, tokens[i].end - tokens[i].start) == 0) &&
				(strlen(key_str) == (size_t)(tokens[i].end - tokens[i].start)))
			{
				int32_t value_len = tokens[i + 1].end - tokens[i + 1].start;

				if (value_len < JSON_MAX_KEY_SIZE)
				{
					strncpy(pc_buffer, kpc_json + tokens[i + 1].start, value_len);
					pc_buffer[value_len] = '\0';
					b_res = true;
					break;
				}
			}
		}
	}
	
	if (b_res)
	{
		return i32_num_tokens;
	}

	return -1;
}