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

TARGETS =	$(BUILD_DIR)/$(UNIT_ARCPROJECT) \

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
# Unit Arcproject
##################################################

UNIT_ARCPROJECT = unit_arcproject
UNIT_ARCPROJECT_INC = $(COMMON_INC) $(TEST_INC)
UNIT_ARCPROJECT_SRCS =	$(UNIT_TESTS_DIR)/$(UNIT_ARCPROJECT).c \
					$(COMMON_SRCS) \
					../src/fsm/arcproject.c

$(BUILD_DIR)/arcproject.o: ../src/fsm/arcproject.c $(BUILD_DIR)
	$(CC) $(COMMON_FLAGS) $(UNIT_ARCPROJECT_INC) -c $< -o $@

# Rule for the unit test object file
$(BUILD_DIR)/$(UNIT_ARCPROJECT).o: $(UNIT_ARCPROJECT_SRCS) $(BUILD_DIR)
	$(CC) $(COMMON_FLAGS) $(TEST_FLAGS) $(UNIT_ARCPROJECT_INC) -c $< -o $@

# Rule for the unit test binary
$(BUILD_DIR)/$(UNIT_ARCPROJECT): $(BUILD_DIR)/$(UNIT_ARCPROJECT).o $(BUILD_DIR)/arcproject.o $(UNITY_OBJS) $(COMMON_SRCS:.c=.o)
	$(CC) $^ -o $@

.PHONY: unit_arcproject
unit_arcproject: $(BUILD_DIR)/$(UNIT_ARCPROJECT)
	./$(BUILD_DIR)/$(UNIT_ARCPROJECT)

##################################################
# Top-Level Commands
##################################################

.PHONY:
all:
	@make unit_arcproject

.PHONY:
clean:
	rm -rf *.o $(JSON_DIR)/*.o $(BUILD_DIR)
