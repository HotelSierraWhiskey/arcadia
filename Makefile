# **************************************************************************** #
#	Compiler settings and build tools
# **************************************************************************** #

CC = arm-none-eabi-gcc
CCLD = arm-none-eabi-ld
SIZE = arm-none-eabi-size
JLINK = JLinkExe

COMMON_FLAGS = \
	-Wall \
	-mcpu=cortex-m0plus \
	-std=gnu17 \
	-nostartfiles \
	-Os \
	-g \
	-fstack-usage

# **************************************************************************** #
#	App paths, includes, C files, and objects
# **************************************************************************** #

SRC	= src
APP_BASENAME = app
APP_BUILD_DIR = $(SRC)/build

# App compiler flags
APP_CFLAGS = 	$(COMMON_FLAGS) \
				-ffunction-sections \
				-fdata-sections \
				-nostartfiles \
				--specs=nosys.specs \
				-T ./toolchain/linkerscript.ld \
				-Wno-switch \
				-Wno-main \
				-Wl,--print-memory-usage \
				-DAUDIO_SWITCH

# App include paths
APP_INC = 	-I../ \
			-I./$(SRC) \
			-I./lib/CMSIS_5/CMSIS/Core/Include \
			-I./lib/samc21/include \
			-I/usr/local/arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi/include \
			-I./$(SRC)/tasks/shell \
			-I./$(SRC)/tasks/drive \
			-I./$(SRC)/tasks/chrono \
			-I./$(SRC)/tasks/media \
			-I./$(SRC)/fsm \
			-I./$(SRC)/json \
			-I./$(SRC)/json/jsmn \
			$(FREERTOS_INC) \
			$(FATFS_INC)

APP_TASKS = 	$(SRC)/tasks/shell/shell.c \
				$(SRC)/tasks/drive/drive.c \
				$(SRC)/tasks/drive/drive_api.c \
				$(SRC)/tasks/chrono/chrono.c \
				$(SRC)/tasks/chrono/chrono_api.c \
				$(SRC)/tasks/media/media.c \
				$(SRC)/tasks/media/media_api.c \

# App C files
APP_VPATH = 	$(APP_TASKS) \
				$(SRC)/main.c  \
				$(SRC)/nvic.c  \
				$(SRC)/sys.c  \
				$(SRC)/io.c \
				$(SRC)/sercom.c \
				$(SRC)/uart.c \
				$(SRC)/shell.c \
				$(SRC)/utils.c \
				$(SRC)/nvmctrl.c \
				$(SRC)/arcadia.c \
				$(SRC)/timer.c \
				$(SRC)/chrono.c \
				$(SRC)/chrono_api.c \
				$(SRC)/media.c \
				$(SRC)/media_api.c \
				$(SRC)/spi.c \
				$(SRC)/sd.c \
				$(SRC)/fsif.c \
				$(SRC)/mempool.c \
				$(SRC)/button.c \
				$(SRC)/ili9488.c \
				$(SRC)/dac.c \
				$(SRC)/exti.c \
				$(SRC)/dma.c \
				$(SRC)/json/json.c \
				$(SRC)/fsm/app_fsm.c \
				$(SRC)/fsm/menu_fsm.c \
				$(SRC)/fsm/story_fsm.c \
				$(SRC)/fsm/arcproject.c \

# Matching .o files from APP_VPATH, and .o files from FreeRTOS and FatFs
APP_OBJECTS = 	$(patsubst %.c,$(APP_BUILD_DIR)/%.o,$(notdir $(wildcard $(APP_VPATH)))) $(FREERTOS_OBJECTS) $(FATFS_OBJECTS)

# **************************************************************************** #
#	FreeRTOS includes, C files, and objects
# **************************************************************************** #

# FreeRTOS Includes
FREERTOS_INC = 	-I./lib/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel \
				-I./lib/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/include \
				-I./lib/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CM0 \

# FreeRTOS C files
FREERTOS_VPATH = 	./lib/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/list.c \
					./lib/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/queue.c \
					./lib/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/tasks.c \
					./lib/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CM0/port.c \
					./lib/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CM0/portasm.c \
					./lib/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CM0/mpu_wrappers_v2_asm.c \

# Matching .o files from FREERTOS_VPATH
FREERTOS_OBJECTS = $(patsubst %.c,$(APP_BUILD_DIR)/%.o,$(notdir $(FREERTOS_VPATH)))

# Build rule for FreeRTOS kernel files
$(APP_BUILD_DIR)/%.o: ./lib/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/%.c
	@echo $@
	@$(CC) $(APP_CFLAGS) $(APP_INC) -c $< -o $@

# Build rule for FreeRTOS arch-specific files
$(APP_BUILD_DIR)/%.o: ./lib/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CM0/%.c
	@echo $@
	@$(CC) $(APP_CFLAGS) $(APP_INC) -c $< -o $@

# Build rule for FreeRTOS memory management files
$(APP_BUILD_DIR)/%.o: ./lib/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/portable/MemMang/%.c
	@echo $@
	@$(CC) $(APP_CFLAGS) $(APP_INC) -c $< -o $@

# **************************************************************************** #
#	FatFs includes, C files, and objects
# **************************************************************************** #

# FatFs Includes
FATFS_INC = 	-I./lib/ff15a/source
# FatFs C files
FATFS_VPATH = 	./lib/ff15a/source/ff.c \
				./lib/ff15a/source/ffunicode.c \

# Matching .o files from FATFS_VPATH
FATFS_OBJECTS = $(patsubst %.c,$(APP_BUILD_DIR)/%.o,$(notdir $(FATFS_VPATH)))

# Build rule for FatFs implementation files
$(APP_BUILD_DIR)/%.o: ./lib/ff15a/source/%.c
	@echo $@
	@$(CC) $(APP_CFLAGS) $(APP_INC) -c $< -o $@

# **************************************************************************** #
#	Application build rules
# **************************************************************************** #

# Rule for app/build
$(APP_BUILD_DIR):
	mkdir -p $(APP_BUILD_DIR)

# .c files in app for app/build
$(APP_BUILD_DIR)/%.o: $(SRC)/%.c | $(APP_BUILD_DIR)
	@echo $@
	@$(CC) $(APP_CFLAGS) $(APP_INC) -c $< -o $@

# .c files in json for app/build
$(APP_BUILD_DIR)/%.o: $(SRC)/json/%.c | $(APP_BUILD_DIR)
	@echo $@
	@$(CC) $(APP_CFLAGS) $(APP_INC) -c $< -o $@

# fsm .c files in common for app/build
$(APP_BUILD_DIR)/%.o: $(SRC)/fsm/%.c | $(APP_BUILD_DIR)
	@echo $@
	@$(CC) $(APP_CFLAGS) $(APP_INC) -c $< -o $@

# task .c files in common for app/build
$(APP_BUILD_DIR)/%.o: $(SRC)/tasks/*/%.c | $(APP_BUILD_DIR)
	@echo $@
	@$(CC) $(APP_CFLAGS) $(APP_INC) -c $< -o $@

# app.o
$(APP_BASENAME).o: $(APP_OBJECTS)
	@echo $@
	@$(CC) $(APP_CFLAGS) -o $(APP_BUILD_DIR)/$(APP_BASENAME).o $(APP_OBJECTS)

# **************************************************************************** #
#	Projects
# **************************************************************************** #

# *************************
#	Dev Board Project (Currently disabled, -D flag needs to be fixed)
# *************************
# app/build/app_dev_board.elf
$(APP_BUILD_DIR)/$(APP_BASENAME)_dev_board.elf: $(APP_OBJECTS)
	@echo $@
	@$(CC) $(APP_CFLAGS) -o $@ $(APP_OBJECTS) -lm

# alias for above
$(APP_BASENAME)_dev_board.elf: $(APP_BUILD_DIR)/$(APP_BASENAME)_dev_board.elf

# alias for app/build/app_dev_board.elf
.PHONY:
compile_dev_board: $(APP_BASENAME)_dev_board.elf

# compile and upload
.PHONY:
upload_dev_board:
	make compile_dev_board && $(JLINK) -CommanderScript ./toolchain/upload_dev_board.jlink

# *************************
#	Audio Switch Project (currently the only project enabled, -D logic needs to be fixed)
# *************************
# app/build/app_audio_switch.elf
$(APP_BUILD_DIR)/$(APP_BASENAME)_audio_switch.elf: $(APP_OBJECTS)
	@echo $@
	@$(CC) $(APP_CFLAGS) -o $@ $(APP_OBJECTS) -lm

# alias for above
$(APP_BASENAME)_audio_switch.elf: $(APP_BUILD_DIR)/$(APP_BASENAME)_audio_switch.elf

# alias for app/build/app_audio_switch.elf
.PHONY:
compile_audio_switch: $(APP_BASENAME)_audio_switch.elf

# compile and upload
.PHONY:
upload_audio_switch:
	make compile_audio_switch && $(JLINK) -CommanderScript ./toolchain/upload_audio_switch.jlink

# **************************************************************************** #
#	Top-level targets
# **************************************************************************** #

# cleans out app/build
.PHONY:
clean:
	rm -rf $(APP_BUILD_DIR)

.PHONY:
size:
	arm-none-eabi-nm --print-size --size-sort -t d $(APP_BUILD_DIR)/$(APP_BASENAME).elf && \
	$(SIZE) $(APP_BUILD_DIR)/$(APP_BASENAME).elf

.PHONY:
elf:
	@arm-none-eabi-readelf -S $(APP_BUILD_DIR)/$(APP_BASENAME).elf

.PHONY:
gdb_server:
	JLinkGDBServer -device ATSAMC21E18 -if SWD -speed 4000
