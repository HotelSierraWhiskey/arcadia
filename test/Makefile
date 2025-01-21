##################################################
# Common
##################################################

CC = gcc

COMMON_FLAGS = \
	-Wall \

##################################################
# Unity & Test Stuff
##################################################

TARGETS = $(UNIT_ARCFILE)

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

$(UNIT_ARCFILE).o: $(UNIT_TESTS_DIR)/$(UNIT_ARCFILE).c
	$(CC) $(COMMON_FLAGS) $(TEST_FLAGS) $(UNITY_INC) -c $< -o $@

$(UNIT_ARCFILE): $(UNIT_ARCFILE).o $(UNITY_OBJS)
	$(CC) $^ -o $@


.PHONY:
clean:
	rm *.o $(TARGETS)