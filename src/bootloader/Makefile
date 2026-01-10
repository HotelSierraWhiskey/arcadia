# **************************************************************************** #
#	Pathing & Directories
# **************************************************************************** #
SRC	=					..
LIB =					$(SRC)/../lib
COMMON =				$(SRC)/common
DRIVERS =				$(SRC)/drivers

BOOTLOADER_BASENAME = 	bootloader
BOOTLOADER_BUILD_DIR = 	build
BOOTLOADER_ELF =		$(BOOTLOADER_BUILD_DIR)/$(BOOTLOADER_BASENAME).elf
BOOTLOADER_HEX =		$(BOOTLOADER_BUILD_DIR)/$(BOOTLOADER_BASENAME).hex

# **************************************************************************** #
#	Bootloader-specific Flags
# **************************************************************************** #
BOOTLOADERSPEC_DEFINES = -DBOOTLOADER

# **************************************************************************** #
#	Compiler Flags
# **************************************************************************** #
BOOTLOADER_CFLAGS = 	$(COMMON_FLAGS) \
						-mthumb \
						-ffunction-sections \
						-fdata-sections \
						-nostartfiles \
						--specs=nosys.specs \
						-T linkerscript.ld \
						-Wno-switch \
						-Wno-main \
						-Wl,--print-memory-usage \
						-Wl,--gc-sections \
						$(BOOTLOADERSPEC_DEFINES) \
						$(COMMON_DEFINES)

# **************************************************************************** #
#	Bootloader Includes
# **************************************************************************** #
BOOTLOADER_INC =		-I. \
						-I/usr/local/arm-gnu-toolchain-14.3.Rel1-x86_64-arm-none-eabi/include \
						-I$(SRC)/ \
						-I$(LIB)/CMSIS_5/CMSIS/Core/Include \
						-I$(LIB)/samc21/include \
						-I$(LIB)/ff15a/source \
						-I$(COMMON) \
						-I$(COMMON)/tinyprintf \
						-I$(DRIVERS) \
						-I$(SRC)/app \
						-I$(SRC)/app/tasks/shell

# **************************************************************************** #
#	Bootloader C Files
# **************************************************************************** #
BOOTLOADER_CFILES =		main.c \
						$(COMMON)/nvic.c \
						$(SRC)/app/sys.c \
						$(DRIVERS)/io.c \
						$(DRIVERS)/sercom.c \
						$(DRIVERS)/uart.c \
						$(DRIVERS)/spi.c \
						$(DRIVERS)/sd.c \
						$(DRIVERS)/fsif.c \
						$(DRIVERS)/nvmctrl.c \
						$(LIB)/ff15a/source/ff.c \
						$(LIB)/ff15a/source/ffunicode.c \
						$(COMMON)/tinyprintf/tinyprintf.c \
						$(COMMON)/printf.c \
						bootloader.c \
						fw_update.c \
						bootutils.c

# **************************************************************************** #
#	App Objects
# **************************************************************************** #

# Replace .c with .o, strip directories, prepend "build/"
BOOTLOADER_OBJS =		$(addprefix $(BOOTLOADER_BUILD_DIR)/, $(notdir $(BOOTLOADER_CFILES:.c=.o)))

# create strings of assignments (e.g. build/arcadia.o=arcadia.c)
BOOTLOADER_OBJ_SRC = 	$(foreach f,$(BOOTLOADER_CFILES),$(BOOTLOADER_BUILD_DIR)/$(notdir $(f:.c=.o))=$(f))

# evaluate those strings to create actual make variables
# now we can do $($@) in our rules
$(foreach m,$(BOOTLOADER_OBJ_SRC),$(eval $(m)))

# **************************************************************************** #
#	Rules
# **************************************************************************** #

$(BOOTLOADER_BUILD_DIR):
	@mkdir -p $@

$(BOOTLOADER_OBJS): | $(BOOTLOADER_BUILD_DIR)
	@echo "CC $($@) -> $@"
	@$(CC) $(BOOTLOADER_CFLAGS) $(BOOTLOADER_INC) -c $($@) -o $@

$(BOOTLOADER_ELF): $(BOOTLOADER_OBJS)
	@echo LD $@
	@$(CC) $(BOOTLOADER_CFLAGS) \
		-o $@ $^

$(BOOTLOADER_HEX): $(BOOTLOADER_ELF)
	@echo OBJCOPY $@
	@$(OBJCOPY) -O ihex $< $@

# **************************************************************************** #
#	Top-level Targets
# **************************************************************************** #

.PHONY: bootloader
bootloader: $(BOOTLOADER_HEX)

.PHONY: clean
clean:
	rm -rf build
