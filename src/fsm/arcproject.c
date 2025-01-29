#include "arcproject.h"
#include "json.h"
#include "utils.h"

/****************************************************************************************************
 *	Retrieves the content file name from the given JSON data.
 *
 * 	This function extracts the value associated with the content file key from the provided JSON 
 * 	string and stores it in the provided buffer.
 *
 * 	@param[in] 	kpc_json The source JSON data
 * 	@param[out] pc_buffer Buffer to store the retrieved content file name (null-terminated)
 * 
 *	@return `true` if the content file name was successfully retrieved and is valid, otherwise `false`
 ****************************************************************************************************/
bool ARCPROJECT_arcfile_get_content_file_name(const char * kpc_json, char * pc_buffer)
{
	ASSERT(kpc_json);
	ASSERT(pc_buffer);

	int32_t i32_bytes_read = JSON_read_value(ARCPROJECT_JSON_KEY_ARCFILE_KEY_CONTENT_STR, JSON_TYPE_STRING, kpc_json, pc_buffer);
	
	if (i32_bytes_read > 0 && strlen(pc_buffer) > 0)
	{
		return true;
	}

	return false;
}

/****************************************************************************************************
 *	Retrieves the number of choices available in the given arcfile JSON data.
 *
 * 	This function counts the number of objects within the "choices" section of the provided JSON 
 * 	string and stores the result in the provided output variable.
 *
 * 	@param[in] 	kpc_json The source JSON data
 * 	@param[out] pu8_num_choices Pointer to store the number of choices found
 * 
 *	@return `true` if the number of choices was successfully retrieved, otherwise `false`
 ****************************************************************************************************/
bool ARCPROJECT_arcfile_get_num_choices(const char * kpc_json, uint8_t * pu8_num_choices)
{
	ASSERT(kpc_json);
	ASSERT(pu8_num_choices);

	return JSON_get_num_objects(ARCPROJECT_JSON_KEY_ARCFILE_KEY_CHOICES_STR, kpc_json, pu8_num_choices);
}

/****************************************************************************************************
 *	Retrieves the key name of a choice at a specified index from the given arcfile JSON data.
 *
 * 	This function extracts the key name of a choice within the "choices" object of the provided JSON 
 * 	string based on the specified index and stores it in the provided buffer. The function considers 
 * 	only the top-level keys within the choices object.
 *
 * 	@param[in] 	kpc_json The source JSON data
 * 	@param[in] 	u8_choice The index of the choice to retrieve (0-based)
 * 	@param[out] pc_buffer Buffer to store the retrieved choice key name (null-terminated)
 * 
 *	@return `true` if the choice key name was successfully retrieved, otherwise `false`
 ****************************************************************************************************/
bool ARCPROJECT_arcfile_get_choice_text(const char * kpc_json, uint8_t u8_choice, char * pc_buffer)
{
	ASSERT(kpc_json);
	ASSERT(pc_buffer);

	return JSON_get_key_from_index(ARCPROJECT_JSON_KEY_ARCFILE_KEY_CHOICES_STR, kpc_json, u8_choice, pc_buffer);
}

/****************************************************************************************************
 *	Retrieves the node value of a choice at a specified index from the given arcfile JSON data.
 *
 * 	This function extracts the integer node value corresponding to a choice within the "choices" 
 * 	object of the provided JSON string based on the specified index and stores it in the provided 
 * 	integer pointer. The function considers only the top-level key-value pairs within the choices 
 * 	object.
 *
 * 	@param[in] 	kpc_json The source JSON data
 * 	@param[in] 	u8_choice The index of the choice to retrieve (0-based)
 * 	@param[out] pi32_node Pointer to store the retrieved choice node value
 * 
 *	@return `true` if the choice node value was successfully retrieved, otherwise `false`
 ****************************************************************************************************/
bool ARCPROJECT_arcfile_get_choice_node(const char * kpc_json, uint8_t u8_choice, int32_t * pi32_node)
{
	ASSERT(kpc_json);
	ASSERT(pi32_node);

	int32_t 	value;
	uint8_t 	u8_num_choices = 0;

	if (ARCPROJECT_arcfile_get_num_choices(kpc_json, &u8_num_choices))
	{
		if (u8_choice < u8_num_choices)
		{
			if (JSON_get_value_from_index(ARCPROJECT_JSON_KEY_ARCFILE_KEY_CHOICES_STR, kpc_json, u8_choice, JSON_TYPE_INT, &value))
			{
				*pi32_node = (uint32_t)value;
				return true;
			}
		}
	}
	return false;
}

/****************************************************************************************************
 *	Retrieves the bookmarked node from the given bookmark.json data.
 *
 * 	This function extracts the integer value corresponding to the bookmarked node and stores it in 
 * 	the provided integer pointer.
 *
 * 	@param[in] 	kpc_json The source JSON data
 * 	@param[out] pi32_node Pointer to store the retrieved bookmarked node value
 * 
 *	@return `true` if the node value was successfully retrieved, otherwise `false`
 ****************************************************************************************************/
bool ARCPROJECT_bookmark_get_node(const char * kpc_json, int32_t * pi32_node)
{
	ASSERT(kpc_json);
	ASSERT(pi32_node);

	int32_t i32_bytes_read = JSON_read_value(ARCPROJECT_JSON_KEY_BOOKMARK_KEY_NODE_STR, JSON_TYPE_INT, kpc_json, pi32_node);

	return i32_bytes_read > 0 ? true : false;
}

/****************************************************************************************************
 *	Retrieves the bookmarked page from the given bookmark.json data.
 *
 * 	This function extracts the integer value corresponding to the bookmarked page and stores it in 
 * 	the provided integer pointer.
 *
 * 	@param[in] 	kpc_json The source JSON data
 * 	@param[out] pi32_node Pointer to store the retrieved bookmarked page value
 * 
 *	@return `true` if the page value was successfully retrieved, otherwise `false`
 ****************************************************************************************************/
bool ARCPROJECT_bookmark_get_page(const char * kpc_json, int32_t * pi32_page)
{
	ASSERT(kpc_json);
	ASSERT(pi32_page);

	int32_t i32_bytes_read = JSON_read_value(ARCPROJECT_JSON_KEY_BOOKMARK_KEY_PAGE_STR, JSON_TYPE_INT, kpc_json, pi32_page);

	return i32_bytes_read > 0 ? true : false;
}

bool ARCPROJECT_bookmark_set_node(char * pc_json, int32_t i32_node)
{
	ASSERT(pc_json);

	return JSON_write_value(ARCPROJECT_JSON_KEY_BOOKMARK_KEY_NODE_STR, JSON_TYPE_INT, pc_json, &i32_node);
}

bool ARCPROJECT_bookmark_set_page(char * pc_json, int32_t i32_page)
{
	ASSERT(pc_json);

	return JSON_write_value(ARCPROJECT_JSON_KEY_BOOKMARK_KEY_PAGE_STR, JSON_TYPE_INT, pc_json, &i32_page);
}

int32_t	ARCPROJECT_get_node_from_arcfile(const char * kpc_fname)
{
	
}
