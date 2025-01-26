#include "unity.h"
#include "unity_fixture.h"
#include "json.h"
#include "arcproject.h"

/****************************************************************************************************
 *	S C A F F O L D I N G
 ****************************************************************************************************/

TEST_GROUP(unit_arcproject);

TEST_SETUP(unit_arcproject)
{
	// Nothing
}

TEST_TEAR_DOWN(unit_arcproject)
{
	UnityConcludeTest(); 
}

/****************************************************************************************************
 *	U N I T   T E S T S
 ****************************************************************************************************/

/**
 *	Tests decoding the content file nominally
 */
TEST(unit_arcproject, decode_arcfile_content_file_name_nominal)
{
	char 			pc_buffer[JSON_BUFFER_MAX_SIZE];
	const char * 	kpc_json = 
	"{"
		"\"content\": \"text_0.txt\","
		"\"choices\":"
		"{"
			"\"choice_one\": 42,"
			"\"choice_two\": 1"
		"}"
	"}";

	TEST_ASSERT(ARCPROJECT_arcfile_get_content_file_name(kpc_json, pc_buffer));
	TEST_ASSERT_EQUAL_STRING("text_0.txt", pc_buffer);
}

/**
 *	Tests decoding the content file when the key itself is missing
 *	`ARCPROJECT_arcfile_get_content_file_name` should fail.
 */
TEST(unit_arcproject, decode_arcfile_content_file_name_key_missing)
{
	char 			pc_buffer[JSON_BUFFER_MAX_SIZE];
	const char * 	kpc_json = 
	"{"
		"\"choices\":"
		"{"
			"\"choice_one\": 42,"
			"\"choice_two\": 1"
		"}"
	"}";

	TEST_ASSERT_FALSE(ARCPROJECT_arcfile_get_content_file_name(kpc_json, pc_buffer));
}

/**
 *	Tests decoding content file when the content file is an empty string
 *	`ARCPROJECT_arcfile_get_content_file_name` should fail.
 */
TEST(unit_arcproject, decode_arcfile_content_file_name_empty_string)
{
	char 			pc_buffer[JSON_BUFFER_MAX_SIZE];
	const char * 	kpc_json = 
	"{"
		"\"content\": \"\""
	"}";

	TEST_ASSERT_FALSE(ARCPROJECT_arcfile_get_content_file_name(kpc_json, pc_buffer));
}

/**
 *	Tests decoding choices nominally
 */
TEST(unit_arcproject, decode_arcfile_num_choices_nominal)
{
	uint8_t 		u8_num_choices = 0;
	const char * 	kpc_json = 
	"{"
		"\"content\": \"text_0.txt\","
		"\"choices\":"
		"{"
			"\"choice_one\": 12,"
			"\"choice_two\": 34,"
			"\"choice_three\": 56,"
			"\"choice_four\": 78"
		"}"
	"}";

	TEST_ASSERT(ARCPROJECT_arcfile_get_num_choices(kpc_json, &u8_num_choices));
	TEST_ASSERT_EQUAL(4, u8_num_choices);
}

/**
 *	Tests decoding choices when no choices exist
 */
TEST(unit_arcproject, decode_arcfile_num_choices_no_choices_exist)
{
	uint8_t 		u8_num_choices = 0;
	const char * 	kpc_json = 
	"{"
		"\"content\": \"text_0.txt\""
	"}";

	TEST_ASSERT_FALSE(ARCPROJECT_arcfile_get_num_choices(kpc_json, &u8_num_choices));
	TEST_ASSERT_EQUAL(0, u8_num_choices);
}

/**
 *	Tests decoding choices when the choices key is an empty object
 */
TEST(unit_arcproject, decode_arcfile_num_choices_empty_object)
{
	uint8_t 		u8_num_choices = 0;
	const char * 	kpc_json = 
	"{"
		"\"content\": \"text_0.txt\","
		"\"choices\":"
		"{"
		"}"
	"}";

	TEST_ASSERT(ARCPROJECT_arcfile_get_num_choices(kpc_json, &u8_num_choices));
	TEST_ASSERT_EQUAL(0, u8_num_choices);
}

/**
 *	Tests indexing choice text and node entries nominally
 */
TEST(unit_arcproject, decode_arcfile_index_choices_nominal)
{
	uint8_t 		u8_num_choices = 0;
	char 			pc_choice_text[JSON_BUFFER_MAX_SIZE];
	int32_t			i32_choice_node = 0;
	const char * 	kpc_json = 
	"{"
		"\"content\": \"text_0.txt\","
		"\"choices\":"
		"{"
			"\"Tell a joke\": 12,"
			"\"Kill! Maim! Burn!\": 34,"
			"\"Offer peace\": 56,"
			"\"Remain silent\": 78"
		"}"
	"}";

	TEST_ASSERT(ARCPROJECT_arcfile_get_num_choices(kpc_json, &u8_num_choices));
	TEST_ASSERT_EQUAL(4, u8_num_choices);

	TEST_ASSERT(ARCPROJECT_arcfile_get_choice_text(kpc_json, 0, pc_choice_text));
	TEST_ASSERT(ARCPROJECT_arcfile_get_choice_node(kpc_json, 0, &i32_choice_node));
	TEST_ASSERT_EQUAL_STRING("Tell a joke", pc_choice_text);
	TEST_ASSERT_EQUAL(12, i32_choice_node);

	TEST_ASSERT(ARCPROJECT_arcfile_get_choice_text(kpc_json, 1, pc_choice_text));
	TEST_ASSERT(ARCPROJECT_arcfile_get_choice_node(kpc_json, 1, &i32_choice_node));
	TEST_ASSERT_EQUAL_STRING("Kill! Maim! Burn!", pc_choice_text);
	TEST_ASSERT_EQUAL(34, i32_choice_node);

	TEST_ASSERT(ARCPROJECT_arcfile_get_choice_text(kpc_json, 2, pc_choice_text));
	TEST_ASSERT(ARCPROJECT_arcfile_get_choice_node(kpc_json, 2, &i32_choice_node));
	TEST_ASSERT_EQUAL_STRING("Offer peace", pc_choice_text);
	TEST_ASSERT_EQUAL(56, i32_choice_node);

	TEST_ASSERT(ARCPROJECT_arcfile_get_choice_text(kpc_json, 3, pc_choice_text));
	TEST_ASSERT(ARCPROJECT_arcfile_get_choice_node(kpc_json, 3, &i32_choice_node));
	TEST_ASSERT_EQUAL_STRING("Remain silent", pc_choice_text);
	TEST_ASSERT_EQUAL(78, i32_choice_node);
}

/**
 *	Tests decoding bookmark nominal
 */
TEST(unit_arcproject, decode_bookmark_nominal)
{
	int32_t			i32_node = 0;
	int32_t			i32_page = 0;
	const char * 	kpc_json = 
	"{"
		"\"node\": 1,"
		"\"page\": 2"
	"}";

	TEST_ASSERT_TRUE(ARCPROJECT_bookmark_get_node(kpc_json, &i32_node));
	TEST_ASSERT_EQUAL(1, i32_node);

	TEST_ASSERT_TRUE(ARCPROJECT_bookmark_get_page(kpc_json, &i32_page));
	TEST_ASSERT_EQUAL(2, i32_page);
}

/****************************************************************************************************
 *	M A I N
 ****************************************************************************************************/

static void run_all_tests(void)
{
	// Arcfile content file decoding tests
	RUN_TEST_CASE(unit_arcproject, decode_arcfile_content_file_name_nominal);
	RUN_TEST_CASE(unit_arcproject, decode_arcfile_content_file_name_key_missing);
	RUN_TEST_CASE(unit_arcproject, decode_arcfile_content_file_name_empty_string);

	// Arcfile choices decoding tests
	RUN_TEST_CASE(unit_arcproject, decode_arcfile_num_choices_nominal);
	RUN_TEST_CASE(unit_arcproject, decode_arcfile_num_choices_no_choices_exist);
	RUN_TEST_CASE(unit_arcproject, decode_arcfile_num_choices_empty_object);
	RUN_TEST_CASE(unit_arcproject, decode_arcfile_index_choices_nominal);

	// Move this to unit_bookmark
	RUN_TEST_CASE(unit_arcproject, decode_bookmark_nominal);
}

int main(int argc, const char ** argv)
{
	return UnityMain(argc, argv, run_all_tests);
}
