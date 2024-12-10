CC = arm-none-eabi-gcc
CCLD = arm-none-eabi-ld
JLINK = JLinkExe

COMMON_FLAGS = \
	-Wall \
	-mcpu=cortex-m0plus \
	-std=c17 \
	-nostartfiles \
	-O0 \
	-g

# **************************************************************************** #
#	A P P   S T U F F
# **************************************************************************** #

APP_BASENAME = app
APP_BUILD_DIR = build

APP_CFLAGS = 	$(COMMON_FLAGS) \
				-ffunction-sections \
				-fdata-sections \
				-nostartfiles \
				--specs=nosys.specs \
				-T linkerscript.ld \
				-Wno-switch \

APP_LDFLAGS = -T linkerscript.ld -lgcc --verbose 

APP_INC = 	-Ilib/CMSIS_5/CMSIS/Core/Include \
			-Ilib/samc21/include \
			-I/usr/local/arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi/include \

APP_VPATH = main.c nvic.c sys.c io.c sercom.c uart.c shell.c sys_time.c utils.c

# matching .o files from APP_VPATH
APP_OBJECTS = 	$(patsubst %.c,$(APP_BUILD_DIR)/%.o,$(notdir $(wildcard $(APP_VPATH))))

# app/build
$(APP_BUILD_DIR):
	mkdir -p $(APP_BUILD_DIR)

# .c files in app for app/build
$(APP_BUILD_DIR)/%.o: %.c | $(APP_BUILD_DIR)
	$(CC) $(APP_CFLAGS) $(APP_INC) -c $< -o $@

# .c files in common for app/build
$(APP_BUILD_DIR)/%.o: $(COMMON_DIR)/%.c | $(APP_BUILD_DIR)
	$(CC) $(APP_CFLAGS) $(APP_INC) -c $< -o $@

# app.o
$(APP_BASENAME).o: $(APP_OBJECTS)
	$(CC) $(APP_CFLAGS) -o $(APP_BUILD_DIR)/$(APP_BASENAME).o $(APP_OBJECTS)

# app/build/app.elf
$(APP_BUILD_DIR)/$(APP_BASENAME).elf: $(APP_OBJECTS)
	$(CC) $(APP_CFLAGS) -o $@ $(APP_OBJECTS)

# alias for above
$(APP_BASENAME).elf: $(APP_BUILD_DIR)/$(APP_BASENAME).elf

# alias for above
.PHONY:
compile_app: $(APP_BASENAME).elf

# cleans out app/build
.PHONY:
clean_app:
	rm -rf $(APP_BUILD_DIR)/*

# compile and upload
.PHONY:
upload_app:
	make compile_app && $(JLINK) -CommanderScript upload.jlink

.PHONY:
gdb_server:
	JLinkGDBServer -device ATSAMC21E18 -if SWD -speed 4000