# **************************************************************************** #
#	Pathing & Directories
# **************************************************************************** #
SRC	=				..
LIB =				$(SRC)/../lib
COMMON =			$(SRC)/common
DRIVERS =			$(SRC)/drivers

APP_BASENAME = 		app
APP_BUILD_DIR = 	build
APP_ELF =			$(APP_BUILD_DIR)/$(APP_BASENAME).elf
APP_HEX =			$(APP_BUILD_DIR)/$(APP_BASENAME).hex

# **************************************************************************** #
#	App-specific Flags
# **************************************************************************** #
APPSPEC_DEFINES = 	-DBUILD_HAS_FS

# **************************************************************************** #
#	Compiler Flags
# **************************************************************************** #
APP_CFLAGS = 		$(COMMON_FLAGS) \
					-mthumb \
					-ffunction-sections \
					-fdata-sections \
					-nostartfiles \
					--specs=nosys.specs \
					-T linkerscript.ld \
					-Wno-switch \
					-Wno-main \
					-Wl,--print-memory-usage \
					$(COMMON_DEFINES) \
					$(APPSPEC_DEFINES)

# **************************************************************************** #
#	App Includes
# **************************************************************************** #

FREERTOS_INC = 		-I$(LIB) \
					-I$(LIB)/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel \
					-I$(LIB)/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/include \
					-I$(LIB)/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CM0 \

FATFS_INC =			-I$(LIB)/ff15a/source

TETHYS_INC =		-I./TethysRT

LIB_INC = 			-I$(LIB) \
					-I$(LIB)/CMSIS_5/CMSIS/Core/Include \
					-I$(LIB)/samc21/include

COMMON_INC =		-I$(COMMON) \
					-I$(COMMON)/tinyprintf \
					-I$(COMMON)/json \
					-I$(COMMON)/json/jsmn

DRIVER_INC =		-I$(DRIVERS)

APP_INC = 			-I./ \
					-I/usr/local/arm-gnu-toolchain-14.3.Rel1-x86_64-arm-none-eabi/include \
					-I./tasks/shell \
					-I./tasks/drive \
					-I./tasks/chrono \
					$(FREERTOS_INC) \
					$(FATFS_INC) \
					$(TETHYS_INC) \
					$(LIB_INC) \
					$(COMMON_INC) \
					$(DRIVER_INC)

# **************************************************************************** #
#	App C Files
# **************************************************************************** #

FREERTOS_CFILES = 	$(LIB)/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/list.c \
					$(LIB)/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/queue.c \
					$(LIB)/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/tasks.c \
					$(LIB)/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CM0/port.c \
					$(LIB)/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CM0/portasm.c \
					$(LIB)/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CM0/mpu_wrappers_v2_asm.c

FATFS_CFILES = 		$(LIB)/ff15a/source/ff.c \
					$(LIB)/ff15a/source/ffunicode.c

TETHYS_CFILES = 	./TethysRT/tethysrt.c

COMMON_CFILES =		$(COMMON)/json/json.c \
					$(COMMON)/tinyprintf/tinyprintf.c \
					$(COMMON)/printf.c \
					$(COMMON)/nvic.c \
					$(COMMON)/button.c \
					$(COMMON)/utils.c

DRIVER_CFILES =		$(DRIVERS)/dac.c \
					$(DRIVERS)/dma.c \
					$(DRIVERS)/exti.c \
					$(DRIVERS)/fsif.c \
					$(DRIVERS)/io.c \
					$(DRIVERS)/nvmctrl.c \
					$(DRIVERS)/sd.c \
					$(DRIVERS)/sercom.c \
					$(DRIVERS)/spi.c \
					$(DRIVERS)/timer.c \
					$(DRIVERS)/uart.c

APP_CFILES = 		arcadia.c \
					main.c \
					mempool.c \
					sys.c \
					tasks/shell/shell.c \
					tasks/shell/shell_printf.c \
					tasks/drive/drive.c \
					tasks/drive/drive_api.c \
					tasks/chrono/chrono.c \
					tasks/chrono/chrono_api.c \
					$(FREERTOS_CFILES) \
					$(FATFS_CFILES) \
					$(TETHYS_CFILES) \
					$(COMMON_CFILES) \
					$(DRIVER_CFILES)

# **************************************************************************** #
#	App Objects
# **************************************************************************** #

# Replace .c with .o, strip directories, prepend "build/"
APP_OBJS =			$(addprefix $(APP_BUILD_DIR)/, $(notdir $(APP_CFILES:.c=.o)))

# create strings of assignments (e.g. build/arcadia.o=arcadia.c)
APP_OBJ_SRC = 		$(foreach f,$(APP_CFILES),$(APP_BUILD_DIR)/$(notdir $(f:.c=.o))=$(f))

# evaluate those strings to create actual make variables
# now we can do $($@) in our rules
$(foreach m,$(APP_OBJ_SRC),$(eval $(m)))

# **************************************************************************** #
#	Rules
# **************************************************************************** #

$(APP_BUILD_DIR):
	@mkdir -p $@

$(APP_OBJS): | $(APP_BUILD_DIR)
	@echo "CC $($@) -> $@"
	@$(CC) $(APP_CFLAGS) $(APP_INC) -c $($@) -o $@

$(APP_ELF): $(APP_OBJS)
	@echo LD $@
	@$(CC) $(APP_CFLAGS) -o $@ $^ -lm

$(APP_HEX): $(APP_ELF)
	@echo OBJCOPY $@
	@$(OBJCOPY) -O ihex $< $@

# **************************************************************************** #
#	Top-level Targets
# **************************************************************************** #

.PHONY: app
app: $(APP_HEX)

.PHONY: clean
clean:
	rm -rf build
