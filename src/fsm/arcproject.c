#include "arcproject.h"
#include "json.h"
#include "utils.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define ARCPROJECT_ARCFILE_EXT 					".arc.json"
#define ARCPROJECT_ARCFILE_EXT_LENGTH			(sizeof(ARCPROJECT_ARCFILE_EXT) - 1)
#define ARCPROJECT_MIN_ARCFILE_FNAME_LENGTH		(12U) // e.g. a_0.arc.json

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

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

/****************************************************************************************************
 *	Sets the bookmarked node in the given `bookmark.json` data.
 *
 *	@param[in] 	pc_json   Pointer to the JSON data buffer to modify.
 *	@param[in] 	i32_node  The node value to set in the JSON.
 * 
 *	@return `true` if the node value was successfully written, otherwise `false`.
 ****************************************************************************************************/
bool ARCPROJECT_bookmark_set_node(char * pc_json, int32_t i32_node)
{
	ASSERT(pc_json);

	return JSON_write_value(ARCPROJECT_JSON_KEY_BOOKMARK_KEY_NODE_STR, JSON_TYPE_INT, pc_json, &i32_node);
}

/****************************************************************************************************
 *	Sets the bookmarked page in the given `bookmark.json` data.
 *
 *	@param[in] 	pc_json   Pointer to the JSON data buffer to modify.
 *	@param[in] 	i32_page  The page value to set in the JSON.
 * 
 *	@return `true` if the page value was successfully written, otherwise `false`.
 ****************************************************************************************************/
bool ARCPROJECT_bookmark_set_page(char * pc_json, int32_t i32_page)
{
	ASSERT(pc_json);

	return JSON_write_value(ARCPROJECT_JSON_KEY_BOOKMARK_KEY_PAGE_STR, JSON_TYPE_INT, pc_json, &i32_page);
}

/****************************************************************************************************
 *	Checks if the given filename follows the Arcadia archive file format.
 *
 *	This function verifies whether a filename matches the expected pattern "<a>_<n>.arc.json",
 *	where `<a>` is a non-empty string, `_` is a separator, and `<n>` is a positive integer.
 *
 *	Validation criteria:
 *	1. The filename must meet the minimum required length (`ARCPROJECT_MIN_ARCFILE_FNAME_LENGTH`).
 *	2. The filename must end with the `.arc.json` extension.
 *	3. There must be an underscore (`_`) before the number.
 *	4. The portion after the underscore must be a valid positive integer.
 *
 *	@param[in] kpc_fname Pointer to the filename string.
 * 
 *	@return `true` if the filename follows the Arcadia archive format, otherwise `false`.
 ****************************************************************************************************/
bool ARCPROJECT_is_arcfile(const char * kpc_fname)
{
	ASSERT(kpc_fname);

	const char * 	kpc_underscore = strrchr(kpc_fname, '_');
	uint32_t 		u32_len = strlen(kpc_fname);
	int32_t 		node;

	// Ensure valid length
	if (u32_len < ARCPROJECT_MIN_ARCFILE_FNAME_LENGTH || u32_len > COMMON_MAX_FNAME_SIZE)
	{
		return false;
	}

	// Ensure filename ends with ".arc.json"
	if (u32_len < ARCPROJECT_ARCFILE_EXT_LENGTH || strcmp(kpc_fname + u32_len - ARCPROJECT_ARCFILE_EXT_LENGTH, ARCPROJECT_ARCFILE_EXT) != 0)
	{
		return false;
	}

	// Ensure there's an underscore and there's a prefix
	if (!kpc_underscore || kpc_underscore == kpc_fname)
	{
		return false;
	}

	// Check if there's a valid number after the kpc_underscore
	if (sscanf(kpc_underscore + 1, "%" PRId32, &node) != 1 || node < 0)
	{
		return false;
	}

	return true;
}

/****************************************************************************************************
 *	Extracts the node number from an arcfile.
 *
 *	@param[in] 	kpc_fname   Pointer to the filename string.
 * 
 *	@return The extracted node number `n` as an `int32_t`.
 ****************************************************************************************************/
int32_t	ARCPROJECT_get_node_from_arcfile(const char * kpc_fname)
{
	ASSERT(kpc_fname);
	ASSERT(ARCPROJECT_is_arcfile(kpc_fname));

	int32_t 		node;
	const char * 	kpc_underscore = strrchr(kpc_fname, '_');

	sscanf(kpc_underscore + 1, "%" PRId32, &node);

	return node;
}
