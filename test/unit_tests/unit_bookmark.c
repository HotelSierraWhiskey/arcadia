#include "unity.h"
#include "unity_fixture.h"
#include "json.h"
#include <string.h>

/****************************************************************************************************
 *	S C A F F O L D I N G
 ****************************************************************************************************/

TEST_GROUP(unit_bookmark);

TEST_SETUP(unit_bookmark)
{
	// Nothing
}

TEST_TEAR_DOWN(unit_bookmark)
{
	UnityConcludeTest(); 
}

/****************************************************************************************************
 *	U N I T   T E S T S
 ****************************************************************************************************/

TEST(unit_bookmark, decode_bookmark_keys_nominal)
{
	int32_t 		i32_num_tokens;
	uint32_t		u32_value;
	const char * 	kpc_json = 
	"{"
		"\"node\": 8,"
		"\"page\": 13"
	"}";

	i32_num_tokens = JSON_read_value(JSON_KEY_ID_BOOKMARK_KEY_NODE, JSON_VALUE_TYPE_INT, kpc_json, &u32_value);

	TEST_ASSERT_EQUAL(5, i32_num_tokens);
	TEST_ASSERT_EQUAL(8, u32_value);

	JSON_read_value(JSON_KEY_ID_BOOKMARK_KEY_PAGE, JSON_VALUE_TYPE_INT, kpc_json, &u32_value);

	TEST_ASSERT_EQUAL(13, u32_value);
}

TEST(unit_bookmark, update_bookmark_keys_nominal)
{
	int32_t 		i32_num_tokens;
	uint32_t		u32_value;
	char pc_json[] = 
	"{"
		"\"node\": 0,"
		"\"page\": 0"
	"}";

	i32_num_tokens = JSON_read_value(JSON_KEY_ID_BOOKMARK_KEY_NODE, JSON_VALUE_TYPE_INT, pc_json, &u32_value);
	TEST_ASSERT_EQUAL(5, i32_num_tokens);
	TEST_ASSERT_EQUAL(0, u32_value);

	JSON_read_value(JSON_KEY_ID_BOOKMARK_KEY_PAGE, JSON_VALUE_TYPE_INT, pc_json, &u32_value);
	TEST_ASSERT_EQUAL(0, u32_value);

	u32_value++;

	JSON_write_value(JSON_KEY_ID_BOOKMARK_KEY_NODE, JSON_VALUE_TYPE_INT, pc_json, &u32_value);
	JSON_write_value(JSON_KEY_ID_BOOKMARK_KEY_PAGE, JSON_VALUE_TYPE_INT, pc_json, &u32_value);

	JSON_read_value(JSON_KEY_ID_BOOKMARK_KEY_NODE, JSON_VALUE_TYPE_INT, pc_json, &u32_value);
	TEST_ASSERT_EQUAL(1, u32_value);

	u32_value = 0;

	JSON_read_value(JSON_KEY_ID_BOOKMARK_KEY_PAGE, JSON_VALUE_TYPE_INT, pc_json, &u32_value);
	TEST_ASSERT_EQUAL(1, u32_value);
}

/****************************************************************************************************
 *	M A I N
 ****************************************************************************************************/

static void run_all_tests(void)
{
	RUN_TEST_CASE(unit_bookmark, decode_bookmark_keys_nominal);
	RUN_TEST_CASE(unit_bookmark, update_bookmark_keys_nominal);
}

int main(int argc, const char ** argv)
{
	return UnityMain(argc, argv, run_all_tests);
}
