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
#	A P P   S T U F F
# **************************************************************************** #

APP_BASENAME = app
APP_BUILD_DIR = build

# App compiler flags
APP_CFLAGS = 	$(COMMON_FLAGS) \
				-ffunction-sections \
				-fdata-sections \
				-nostartfiles \
				--specs=nosys.specs \
				-T linkerscript.ld \
				-Wno-switch \
				-Wno-main \

# App include paths
APP_INC = 	-I./ \
			-Ilib/CMSIS_5/CMSIS/Core/Include \
			-Ilib/samc21/include \
			-I/usr/local/arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi/include \
			$(FREERTOS_INC)

# App C files
APP_VPATH = main.c nvic.c sys.c io.c sercom.c uart.c shell.c chronos.c utils.c nvmctrl.c arcadia.c drive.c

# Matching .o files from APP_VPATH
APP_OBJECTS = 	$(patsubst %.c,$(APP_BUILD_DIR)/%.o,$(notdir $(wildcard $(APP_VPATH)))) $(FREERTOS_OBJECTS)

# FreeRTOS Includes
FREERTOS_INC = 	-IFreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel \
				-IFreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/include \
				-IFreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CM0 \

# FreeRTOS C files
FREERTOS_VPATH = 	FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/list.c \
					FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/queue.c \
					FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/tasks.c \
					FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CM0/port.c \
					FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CM0/portasm.c \
					FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CM0/mpu_wrappers_v2_asm.c \

# FreeRTOS .o object names
FREERTOS_OBJECTS = $(patsubst %.c,$(APP_BUILD_DIR)/%.o,$(notdir $(FREERTOS_VPATH)))

# Rule for app/build
$(APP_BUILD_DIR):
	mkdir -p $(APP_BUILD_DIR)

# Build rule for FreeRTOS kernel files
$(APP_BUILD_DIR)/%.o: FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/%.c
	@echo $@
	@$(CC) $(APP_CFLAGS) $(APP_INC) -c $< -o $@

# Build rule for FreeRTOS arch-specific files
$(APP_BUILD_DIR)/%.o: FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CM0/%.c
	@echo $@
	@$(CC) $(APP_CFLAGS) $(APP_INC) -c $< -o $@

# Build rule for FreeRTOS memory management files
$(APP_BUILD_DIR)/%.o: FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/portable/MemMang/%.c
	@echo $@
	@$(CC) $(APP_CFLAGS) $(APP_INC) -c $< -o $@

# .c files in app for app/build
$(APP_BUILD_DIR)/%.o: %.c | $(APP_BUILD_DIR)
	@echo $@
	@$(CC) $(APP_CFLAGS) $(APP_INC) -c $< -o $@

# .c files in common for app/build
$(APP_BUILD_DIR)/%.o: $(COMMON_DIR)/%.c | $(APP_BUILD_DIR)
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

# alias for above
.PHONY:
compile_app: $(APP_BASENAME).elf

# cleans out app/build
.PHONY:
clean_app:
	rm -rf $(APP_BUILD_DIR)/*

.PHONY:
size:
	@$(SIZE) $(APP_BUILD_DIR)/$(APP_BASENAME).elf

# compile and upload
.PHONY:
upload_app:
	make compile_app && $(JLINK) -CommanderScript upload.jlink

.PHONY:
gdb_server:
	JLinkGDBServer -device ATSAMC21E18 -if SWD -speed 4000