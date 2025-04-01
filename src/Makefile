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
	-std=c17 \
	-nostartfiles \
	-Os \
	-g \
	-fstack-usage

# **************************************************************************** #
#	App paths, includes, C files, and objects
# **************************************************************************** #

APP_BASENAME = app
APP_BUILD_DIR = build

# App compiler flags
APP_CFLAGS = 	$(COMMON_FLAGS) \
				-ffunction-sections \
				-fdata-sections \
				-nostartfiles \
				--specs=nosys.specs \
				-T ../toolchain/linkerscript.ld \
				-Wno-switch \
				-Wno-main \
				-Wl,--print-memory-usage

# App include paths
APP_INC = 	-I./ \
			-I../lib/CMSIS_5/CMSIS/Core/Include \
			-I../lib/samc21/include \
			-I/usr/local/arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi/include \
			-I./fsm \
			-I./json \
			-I./json/jsmn \
			$(FREERTOS_INC) \
			$(FATFS_INC)

# App C files
APP_VPATH = 	main.c  \
				nvic.c  \
				sys.c  \
				io.c \
				sercom.c \
				uart.c \
				shell.c \
				utils.c \
				nvmctrl.c \
				arcadia.c \
				drive.c \
				drive_api.c \
				timer.c \
				chrono.c \
				chrono_api.c \
				spi.c \
				sd.c \
				fsif.c \
				mempool.c \
				button.c \
				ili9488.c \
				dac.c \
				json/json.c \
				fsm/app_fsm.c \
				fsm/menu_fsm.c \
				fsm/story_fsm.c \
				fsm/arcproject.c \

# Matching .o files from APP_VPATH, and .o files from FreeRTOS and FatFs
APP_OBJECTS = 	$(patsubst %.c,$(APP_BUILD_DIR)/%.o,$(notdir $(wildcard $(APP_VPATH)))) $(FREERTOS_OBJECTS) $(FATFS_OBJECTS)

# **************************************************************************** #
#	FreeRTOS includes, C files, and objects
# **************************************************************************** #

# FreeRTOS Includes
FREERTOS_INC = 	-I../lib/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel \
				-I../lib/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/include \
				-I../lib/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CM0 \

# FreeRTOS C files
FREERTOS_VPATH = 	../lib/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/list.c \
					../lib/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/queue.c \
					../lib/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/tasks.c \
					../lib/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CM0/port.c \
					../lib/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CM0/portasm.c \
					../lib/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CM0/mpu_wrappers_v2_asm.c \

# Matching .o files from FREERTOS_VPATH
FREERTOS_OBJECTS = $(patsubst %.c,$(APP_BUILD_DIR)/%.o,$(notdir $(FREERTOS_VPATH)))

# Build rule for FreeRTOS kernel files
$(APP_BUILD_DIR)/%.o: ../lib/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/%.c
	@echo $@
	@$(CC) $(APP_CFLAGS) $(APP_INC) -c $< -o $@

# Build rule for FreeRTOS arch-specific files
$(APP_BUILD_DIR)/%.o: ../lib/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CM0/%.c
	@echo $@
	@$(CC) $(APP_CFLAGS) $(APP_INC) -c $< -o $@

# Build rule for FreeRTOS memory management files
$(APP_BUILD_DIR)/%.o: ../lib/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/portable/MemMang/%.c
	@echo $@
	@$(CC) $(APP_CFLAGS) $(APP_INC) -c $< -o $@

# **************************************************************************** #
#	FatFs includes, C files, and objects
# **************************************************************************** #

# FatFs Includes
FATFS_INC = 	-I../lib/ff15a/source
# FatFs C files
FATFS_VPATH = 	../lib/ff15a/source/ff.c \
				../lib/ff15a/source/ffunicode.c \

# Matching .o files from FATFS_VPATH
FATFS_OBJECTS = $(patsubst %.c,$(APP_BUILD_DIR)/%.o,$(notdir $(FATFS_VPATH)))

# Build rule for FatFs implementation files
$(APP_BUILD_DIR)/%.o: ../lib/ff15a/source/%.c
	@echo $@
	@$(CC) $(APP_CFLAGS) $(APP_INC) -c $< -o $@

# **************************************************************************** #
#	Application build rules
# **************************************************************************** #

# Rule for app/build
$(APP_BUILD_DIR):
	mkdir -p $(APP_BUILD_DIR)

# .c files in app for app/build
$(APP_BUILD_DIR)/%.o: %.c | $(APP_BUILD_DIR)
	@echo $@
	@$(CC) $(APP_CFLAGS) $(APP_INC) -c $< -o $@

# .c files in json for app/build
$(APP_BUILD_DIR)/%.o: json/%.c | $(APP_BUILD_DIR)
	@echo $@
	@$(CC) $(APP_CFLAGS) $(APP_INC) -c $< -o $@

# fsm .c files in common for app/build
$(APP_BUILD_DIR)/%.o: fsm/%.c | $(APP_BUILD_DIR)
	@echo $@
	@$(CC) $(APP_CFLAGS) $(APP_INC) -c $< -o $@

# app.o
$(APP_BASENAME).o: $(APP_OBJECTS)
	@echo $@
	@$(CC) $(APP_CFLAGS) -o $(APP_BUILD_DIR)/$(APP_BASENAME).o $(APP_OBJECTS)

# app/build/app.elf
$(APP_BUILD_DIR)/$(APP_BASENAME).elf: $(APP_OBJECTS)
	@echo $@
	@$(CC) $(APP_CFLAGS) -o $@ $(APP_OBJECTS)

# alias for above
$(APP_BASENAME).elf: $(APP_BUILD_DIR)/$(APP_BASENAME).elf

# **************************************************************************** #
#	Top-level targets
# **************************************************************************** #

# alias for app/build/app.elf
.PHONY:
compile_app: $(APP_BASENAME).elf

# cleans out app/build
.PHONY:
clean_app:
	rm -rf $(APP_BUILD_DIR)

.PHONY:
size:
	arm-none-eabi-nm --print-size --size-sort -t d $(APP_BUILD_DIR)/$(APP_BASENAME).elf && \
	$(SIZE) $(APP_BUILD_DIR)/$(APP_BASENAME).elf

.PHONY:
elf:
	@arm-none-eabi-readelf -S $(APP_BUILD_DIR)/$(APP_BASENAME).elf

# compile and upload
.PHONY:
upload_app:
	make compile_app && $(JLINK) -CommanderScript ../toolchain/upload.jlink

.PHONY:
gdb_server:
	JLinkGDBServer -device ATSAMC21E18 -if SWD -speed 4000
