#ifndef ARCPROJECT_H
#define ARCPROJECT_H

#include "common.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define ARCPROJECT_BOOKMARK_FILENAME					"bookmark.json"
#define ARCPROJECT_JSON_KEY_ARCFILE_KEY_CONTENT_STR		"content"
#define ARCPROJECT_JSON_KEY_ARCFILE_KEY_CHOICES_STR		"choices"
#define ARCPROJECT_JSON_KEY_BOOKMARK_KEY_NODE_STR		"node"
#define ARCPROJECT_JSON_KEY_BOOKMARK_KEY_PAGE_STR		"page"

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

bool        ARCPROJECT_arcfile_get_content_file_name    (const char * kpc_json, char * pc_buffer);
bool        ARCPROJECT_arcfile_get_num_choices          (const char * kpc_json, uint8_t * pu8_num_choices);
bool        ARCPROJECT_arcfile_get_choice_text          (const char * kpc_json, uint8_t u8_choice, char * pc_buffer);
bool 		ARCPROJECT_arcfile_get_choice_node			(const char * kpc_json, uint8_t u8_choice, int32_t * pi32_node);
bool        ARCPROJECT_bookmark_get_node                (const char * kpc_json, int32_t * pi32_node);
bool        ARCPROJECT_bookmark_set_node                (char * pc_json, int32_t i32_node);
bool        ARCPROJECT_bookmark_get_page                (const char * kpc_json, int32_t * pi32_page);
bool        ARCPROJECT_bookmark_set_page                (char * pc_json, int32_t i32_page);
bool 		ARCPROJECT_is_arcfile						(const char * kpc_fname);
int32_t		ARCPROJECT_get_node_from_arcfile			(const char * kpc_fname);

#endif // ARCPROJECT_H
