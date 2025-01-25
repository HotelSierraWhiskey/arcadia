#include "arcproject.h"
#include "json.h"
#include "utils.h"

bool ARCPROJECT_arcfile_get_content_file_name(const char * kpc_json, char * pc_buffer)
{
	ASSERT(kpc_json);
	ASSERT(pc_buffer);

	int32_t i32_bytes_read = JSON_read_value(ARCPROJECT_JSON_KEY_ARCFILE_KEY_CONTENT_STR, JSON_VALUE_TYPE_STRING, kpc_json, pc_buffer);
	
	if (i32_bytes_read > 0 && strlen(pc_buffer) > 0)
	{
		return true;
	}

	return false;
}

bool ARCPROJECT_arcfile_get_num_choices(const char * kpc_json, uint8_t * pu8_num_choices)
{
	ASSERT(kpc_json);
	ASSERT(pu8_num_choices);

	return JSON_get_num_objects(ARCPROJECT_JSON_KEY_ARCFILE_KEY_CHOICES_STR, kpc_json, pu8_num_choices);
}

bool ARCPROJECT_arcfile_get_choice_text(const char * kpc_json, uint8_t u8_choice, char * pc_buffer)
{
	ASSERT(kpc_json);
	ASSERT(pc_buffer);
}

bool ARCPROJECT_arcfile_get_choice_node(const char * kpc_json, uint32_t * p_node)
{
	ASSERT(kpc_json);
	ASSERT(p_node);
}

bool ARCPROJECT_bookmark_get_node(const char * kpc_json, uint32_t * p_node)
{
	ASSERT(kpc_json);
	ASSERT(p_node);
}

bool ARCPROJECT_bookmark_get_page(const char * kpc_json, uint32_t * p_page)
{
	ASSERT(kpc_json);
	ASSERT(p_page);
}
