##################################################
# Common
##################################################

CC = gcc

COMMON_FLAGS = \
	-Wall \
	-Wno-switch \
	-DUNIT_TEST \

COMMON_INC = 	-I../ \
				-I../src \
				-I../src/json \
				-I../src/json/jsmn \
				-I../src/fsm \

COMMON_SRCS = ../src/json/json.c

##################################################
# Common Rules
##################################################

JSON_DIR = ../src/json

# Rule for JSON stuff
$(JSON_DIR)/%.o: $(JSON_DIR)/%.c
	$(CC) $(COMMON_FLAGS) $(COMMON_INC) -c $< -o $@

##################################################
# Unity & Test Stuff
##################################################

TARGETS =	$(BUILD_DIR)/$(UNIT_ARCFILE) \
			$(BUILD_DIR)/$(UNIT_BOOKMARK)

# Where unit test binaries and object files are placed
BUILD_DIR = build

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

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
UNIT_ARCFILE_INC = $(COMMON_INC) $(TEST_INC)
UNIT_ARCFILE_SRCS = $(UNIT_TESTS_DIR)/$(UNIT_ARCFILE).c $(COMMON_SRCS)

# Rule for the unit test object file
$(BUILD_DIR)/$(UNIT_ARCFILE).o: $(UNIT_ARCFILE_SRCS) $(BUILD_DIR)
	$(CC) $(COMMON_FLAGS) $(TEST_FLAGS) $(UNIT_ARCFILE_INC) -c $< -o $@

# Rule for the unit test binary
$(BUILD_DIR)/$(UNIT_ARCFILE): $(BUILD_DIR)/$(UNIT_ARCFILE).o $(UNITY_OBJS) $(COMMON_SRCS:.c=.o)
	$(CC) $^ -o $@ && ./$(BUILD_DIR)/$(UNIT_ARCFILE)

.PHONY: unit_arcfile
unit_arcfile: $(BUILD_DIR)/$(UNIT_ARCFILE)
	./$(BUILD_DIR)/$(UNIT_ARCFILE)

##################################################
# Unit Bookmark
##################################################

UNIT_BOOKMARK = unit_bookmark
UNIT_BOOKMARK_INC = $(COMMON_INC) $(TEST_INC)
UNIT_BOOKMARK_SRCS = $(UNIT_TESTS_DIR)/$(UNIT_BOOKMARK).c $(COMMON_SRCS)

# Rule for the unit test object file
$(BUILD_DIR)/$(UNIT_BOOKMARK).o: $(UNIT_BOOKMARK_SRCS) $(BUILD_DIR)
	$(CC) $(COMMON_FLAGS) $(TEST_FLAGS) $(UNIT_BOOKMARK_INC) -c $< -o $@

# Rule for the unit test binary
$(BUILD_DIR)/$(UNIT_BOOKMARK): $(BUILD_DIR)/$(UNIT_BOOKMARK).o $(UNITY_OBJS) $(COMMON_SRCS:.c=.o)
	$(CC) $^ -o $@ && ./$(BUILD_DIR)/$(UNIT_BOOKMARK)

.PHONY: unit_bookmark
unit_bookmark: $(BUILD_DIR)/$(UNIT_BOOKMARK)
	./$(BUILD_DIR)/$(UNIT_BOOKMARK)

##################################################
# Top-Level Commands
##################################################

.PHONY:
all: $(TARGETS)

.PHONY:
clean:
	rm -rf *.o $(JSON_DIR)/*.o $(BUILD_DIR)
