##################################################
# Common
##################################################

CC = gcc

COMMON_FLAGS = \
	-Wall \
	-Wno-switch \
	-DUNIT_TEST \

COMMON_INC = -I../../

##################################################
# Unity & Test Stuff
##################################################

TARGETS = $(UNIT_ARCFILE) $(UNIT_BOOKMARK)

# Unity flags, includes, srcs
UNITY_FLAGS = -DUNITY_SKIP_DEFAULT_RUNNER -DUNITY_INCLUDE_PRINT_FORMATTED -DUNITY_OUTPUT_COLOR

UNITY_INC = -IUnity/src \
            -IUnity/extras/fixture/src \
            -IUnity/extras/memory/src

UNITY_SRCS = 	Unity/src/unity.c \
				Unity/extras/fixture/src/unity_fixture.c \
				Unity/extras/memory/src/unity_memory.c \

# All testing-related flags, includes, srcs
UNIT_TESTS_DIR = unit_tests

TEST_FLAGS = $(UNITY_FLAGS)

TEST_INC = $(UNITY_INC)

TEST_SRCS = $(UNITY_SRCS)

UNITY_OBJS = $(TEST_SRCS:.c=.o)

%.o: %.c
	$(CC) $(COMMON_FLAGS) $(TEST_FLAGS) $(TEST_INC) -c $< -o $@

##################################################
# Unit Arcfile
##################################################

UNIT_ARCFILE = unit_arcfile
UNIT_ARCFILE_INC = $(TEST_INC) -I../ -I../json -I../json/jsmn
JSON_DIR = ../json
JSON_SRCS = $(JSON_DIR)/json.c
UNIT_ARCFILE_SRCS = $(UNIT_TESTS_DIR)/$(UNIT_ARCFILE).c $(JSON_SRCS)

# Rule for JSON stuff
$(JSON_DIR)/%.o: $(JSON_DIR)/%.c
	$(CC) $(COMMON_FLAGS) $(UNIT_ARCFILE_INC) -c $< -o $@

# Rule for the unit test object file
$(UNIT_ARCFILE).o: $(UNIT_TESTS_DIR)/$(UNIT_ARCFILE).c $(JSON_SRCS)
	$(CC) $(COMMON_FLAGS) $(TEST_FLAGS) $(UNITY_INC) $(UNIT_ARCFILE_INC) -c $< -o $@

# Rule for the unit test binary
$(UNIT_ARCFILE): $(UNIT_ARCFILE).o $(UNITY_OBJS) $(JSON_SRCS:.c=.o)
	$(CC) $^ -o $@ && ./$(UNIT_ARCFILE)

##################################################
# Unit Bookmark
##################################################

UNIT_BOOKMARK = unit_bookmark
UNIT_BOOKMARK_INC = $(TEST_INC) -I../ -I../json -I../json/jsmn
JSON_DIR = ../json
JSON_SRCS = $(JSON_DIR)/json.c
UNIT_BOOKMARK_SRCS = $(UNIT_TESTS_DIR)/$(UNIT_BOOKMARK).c $(JSON_SRCS)

# Rule for JSON stuff
$(JSON_DIR)/%.o: $(JSON_DIR)/%.c
	$(CC) $(COMMON_FLAGS) $(UNIT_BOOKMARK_INC) -c $< -o $@

# Rule for the unit test object file
$(UNIT_BOOKMARK).o: $(UNIT_TESTS_DIR)/$(UNIT_BOOKMARK).c $(JSON_SRCS)
	$(CC) $(COMMON_FLAGS) $(TEST_FLAGS) $(UNITY_INC) $(UNIT_BOOKMARK_INC) -c $< -o $@

# Rule for the unit test binary
$(UNIT_BOOKMARK): $(UNIT_BOOKMARK).o $(UNITY_OBJS) $(JSON_SRCS:.c=.o)
	$(CC) $^ -o $@ && ./$(UNIT_BOOKMARK)


.PHONY:
clean:
	rm *.o $(JSON_DIR)/*.o $(TARGETS)