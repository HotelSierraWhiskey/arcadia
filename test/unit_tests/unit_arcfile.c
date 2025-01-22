#include "unity.h"
#include "unity_fixture.h"
#include "json.h"
#include <string.h>

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

TEST(unit_arcfile, decode_arcfile_keys_nominal)
{
	int32_t 		i32_num_tokens;
	char 			pc_buffer[JSON_MAX_KEY_SIZE];
	const char * 	pc_json = 
	"{"
		"\"content\": \"text_0.txt\","
		"\"choices\":"
		"{"
			"\"choice_one\": 42,"
			"\"choice_two\": 1"
		"}"
	"}";

	i32_num_tokens = JSON_value_from_key(JSON_KEY_ARCFILE_KEY_CONTENT, pc_json, strlen(pc_json), pc_buffer);

	TEST_ASSERT_EQUAL(9, i32_num_tokens);
	TEST_ASSERT_EQUAL_STRING("text_0.txt", pc_buffer);

	JSON_value_from_key(JSON_KEY_ARCFILE_KEY_CHOICES, pc_json, strlen(pc_json), pc_buffer);

	TEST_ASSERT_EQUAL_STRING("{\"choice_one\": 42,\"choice_two\": 1}", pc_buffer);
}

/****************************************************************************************************
 *	M A I N
 ****************************************************************************************************/

static void run_all_tests(void)
{
	RUN_TEST_CASE(unit_arcfile, decode_arcfile_keys_nominal);
}

int main(int argc, const char ** argv)
{
	return UnityMain(argc, argv, run_all_tests);
}
