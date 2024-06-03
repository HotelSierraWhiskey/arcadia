# **************************************************************************** #
# C O M M O N   S T U F F
# **************************************************************************** #

CC = arm-none-eabi-gcc
CCLD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

BUILD_DIR = build
COMMON_DIR = common

COMMON_FLAGS = \
	-Wall \
	-mcpu=cortex-m0plus \
	-std=c17 \
	-nostartfiles \
	-Os

OUTFILE = firmware.hex

# **************************************************************************** #
# A P P   S T U F F
# **************************************************************************** #

APP_BASENAME = app
APP_BASEDIR = $(APP_BASENAME)
APP_BUILD_DIR = $(APP_BASEDIR)/build

APP_CFLAGS = $(COMMON_FLAGS) \
	-ffunction-sections \
	-fdata-sections \

APP_LDFLAGS = -T linkerscripts/app.ld

APP_INC = -Icommon/cmsis/samd21a/include \
	-Icommon \

APP_VPATH = $(APP_BASEDIR)/main.c \
	$(COMMON_DIR)/nvic.c \
	$(COMMON_DIR)/io.c \
	$(COMMON_DIR)/button.c \
	$(COMMON_DIR)/clock.c \
	$(COMMON_DIR)/error.c \

# matching .o files from APP_VPATH
APP_OBJECTS = $(patsubst %.c,$(APP_BUILD_DIR)/%.o,$(notdir $(wildcard $(APP_VPATH))))

# app/build
$(APP_BUILD_DIR):
	mkdir -p $(APP_BUILD_DIR)

# .c files in app for app/build
$(APP_BUILD_DIR)/%.o: $(APP_BASEDIR)/%.c | $(APP_BUILD_DIR)
	$(CC) $(APP_CFLAGS) $(APP_INC) -c $< -o $@

# .c files in common for app/build
$(APP_BUILD_DIR)/%.o: $(COMMON_DIR)/%.c | $(APP_BUILD_DIR)
	$(CC) $(APP_CFLAGS) $(APP_INC) -c $< -o $@

# app.o
$(APP_BASENAME).o: $(APP_OBJECTS)
	$(CCLD) $(APP_LDFLAGS) -o $(APP_BUILD_DIR)/$(APP_BASENAME).o $(APP_OBJECTS)

# app/build/app.elf
$(APP_BUILD_DIR)/$(APP_BASENAME).elf: $(APP_OBJECTS)
	$(CCLD) $(APP_LDFLAGS) -o $@ $(APP_OBJECTS)

# alias for above
$(APP_BASENAME).elf: $(APP_BUILD_DIR)/$(APP_BASENAME).elf

# alias for above
.PHONY:
compile_app: $(APP_BASENAME).elf

# cleans out app/build
.PHONY:
clean_app:
	rm -rf $(APP_BUILD_DIR)/*

# **************************************************************************** #
# B O O T L O A D E R   S T U F F
# **************************************************************************** #

BOOTLOADER_BASENAME = bootloader
BOOTLOADER_BASEDIR = $(BOOTLOADER_BASENAME)
BOOTLOADER_BUILD_DIR = $(BOOTLOADER_BASEDIR)/build

BOOTLOADER_CFLAGS = $(COMMON_FLAGS) \
	-ffunction-sections \
	-fdata-sections \

BOOTLOADER_LDFLAGS = -T linkerscripts/bootloader.ld

BOOTLOADER_INC = -Icommon/cmsis/samd21a/include \
	-Icommon \

BOOTLOADER_VPATH = $(BOOTLOADER_BASEDIR)/main.c \
	$(COMMON_DIR)/nvic.c

# matching .o files from BOOTLOADER_VPATH
BOOTLOADER_OBJECTS = $(patsubst %.c,$(BOOTLOADER_BUILD_DIR)/%.o,$(notdir $(wildcard $(BOOTLOADER_VPATH))))

# bootloader/build
$(BOOTLOADER_BUILD_DIR):
	mkdir -p $(BOOTLOADER_BUILD_DIR)

# .c files in bootloader for bootloader/build
$(BOOTLOADER_BUILD_DIR)/%.o: $(BOOTLOADER_BASEDIR)/%.c | $(BOOTLOADER_BUILD_DIR)
	$(CC) $(BOOTLOADER_CFLAGS) $(BOOTLOADER_INC) -c $< -o $@

# .c files in common for bootloader/build
$(BOOTLOADER_BUILD_DIR)/%.o: $(COMMON_DIR)/%.c | $(BOOTLOADER_BUILD_DIR)
	$(CC) $(BOOTLOADER_CFLAGS) $(BOOTLOADER_INC) -c $< -o $@

# bootloader.o
$(BOOTLOADER_BASENAME).o: $(BOOTLOADER_OBJECTS)
	$(CCLD) $(BOOTLOADER_LDFLAGS) -o $(BOOTLOADER_BUILD_DIR)/$(BOOTLOADER_BASENAME).o $(BOOTLOADER_OBJECTS)

# bootloader/build/bootloader.elf
$(BOOTLOADER_BUILD_DIR)/$(BOOTLOADER_BASENAME).elf: $(BOOTLOADER_OBJECTS)
	$(CCLD) $(BOOTLOADER_LDFLAGS) -o $@ $(BOOTLOADER_OBJECTS)

# alias for above
$(BOOTLOADER_BASENAME).elf: $(BOOTLOADER_BUILD_DIR)/$(BOOTLOADER_BASENAME).elf

# alias for above
.PHONY:
compile_bootloader: $(BOOTLOADER_BASENAME).elf

# cleans out bootloader/build
.PHONY:
clean_bootloader:
	rm -rf $(BOOTLOADER_BUILD_DIR)/*

# **************************************************************************** #
# U N I F I E D   F I R M W A R E   S T U F F
# **************************************************************************** #

unified.elf: bootloader.o app.o
	$(CCLD) -T linkerscripts/unified.ld -o build/unified.elf bootloader/build/bootloader.o app/build/app.o

.PHONY:
clean: clean_app clean_bootloader

