#include "unity.h"
#include "unity_fixture.h"
#include "json.h"
#include "arcproject.h"

/****************************************************************************************************
 *	S C A F F O L D I N G
 ****************************************************************************************************/

TEST_GROUP(unit_arcfile);

TEST_SETUP(unit_arcfile)
{
	// Nothing
}

TEST_TEAR_DOWN(unit_arcfile)
{
	UnityConcludeTest(); 
}

/****************************************************************************************************
 *	U N I T   T E S T S
 ****************************************************************************************************/

TEST(unit_arcfile, decode_arcfile_content_file_name_nominal)
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

TEST(unit_arcfile, decode_arcfile_content_file_name_key_missing)
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

TEST(unit_arcfile, decode_arcfile_content_file_name_empty_string)
{
	char 			pc_buffer[JSON_BUFFER_MAX_SIZE];
	const char * 	kpc_json = 
	"{"
		"\"content\": \"\""
	"}";

	TEST_ASSERT_FALSE(ARCPROJECT_arcfile_get_content_file_name(kpc_json, pc_buffer));
}

TEST(unit_arcfile, decode_arcfile_num_choices_nominal)
{
	char 			pc_buffer[JSON_BUFFER_MAX_SIZE];
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
	uint8_t 		u8_num_choices = 0;

	TEST_ASSERT(ARCPROJECT_arcfile_get_num_choices(kpc_json, &u8_num_choices));
	TEST_ASSERT_EQUAL(4, u8_num_choices);
}

/****************************************************************************************************
 *	M A I N
 ****************************************************************************************************/

static void run_all_tests(void)
{
	RUN_TEST_CASE(unit_arcfile, decode_arcfile_content_file_name_nominal);
	RUN_TEST_CASE(unit_arcfile, decode_arcfile_content_file_name_key_missing);
	RUN_TEST_CASE(unit_arcfile, decode_arcfile_content_file_name_empty_string);
	RUN_TEST_CASE(unit_arcfile, decode_arcfile_num_choices_nominal);
}

int main(int argc, const char ** argv)
{
	return UnityMain(argc, argv, run_all_tests);
}
