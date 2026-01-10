# **************************************************************************** #
#	Compiler Settings & Build Tools
# **************************************************************************** #

ARM_GNU_TOOLCHAIN =		/opt/arm_gnu_toolchain
CC =					$(ARM_GNU_TOOLCHAIN)/arm-none-eabi-gcc
CCLD =					$(ARM_GNU_TOOLCHAIN)/arm-none-eabi-ld
SIZE =					$(ARM_GNU_TOOLCHAIN)/arm-none-eabi-size
OBJCOPY =				$(ARM_GNU_TOOLCHAIN)/arm-none-eabi-objcopy
SREC_CAT =				srec_cat
JLINK =					JLinkExe

COMMON_FLAGS =	\
				-Wall \
				-mcpu=cortex-m0plus \
				-std=gnu23 \
				-nostartfiles \
				-Os \
				-g \
				-ffunction-sections \
				-fdata-sections \
				-fstack-usage \
				--specs=nano.specs

export CC CCLD SIZE JLINK COMMON_FLAGS OBJCOPY

# **************************************************************************** #
#	Paths
# **************************************************************************** #

SRC =				src
BUILD_DIR =			$(SRC)/build
APP_HEX =			$(SRC)/app/build/app.hex
BOOTLOADER_HEX =	$(SRC)/bootloader/build/bootloader.hex
COMBINED_HEX =		$(BUILD_DIR)/combined.hex

# **************************************************************************** #
#	Rules
# **************************************************************************** #

$(BUILD_DIR):
	@mkdir -p $@

$(COMBINED_HEX): | $(BUILD_DIR)
$(COMBINED_HEX): bootloader app
	@echo SREC_CAT $@
	@$(SREC_CAT) $(BOOTLOADER_HEX) -intel $(APP_HEX) -intel -o $@ -intel

# **************************************************************************** #
#	Top-level Targets
# **************************************************************************** #

.PHONY:
bootloader:
	$(MAKE) -C src/bootloader/ bootloader

.PHONY:
app:
	$(MAKE) -C src/app/ app

.PHONY:
combined: $(COMBINED_HEX)

.PHONY:
upload: $(COMBINED_HEX)
	$(JLINK) -CommanderScript upload.jlink

# **************************************************************************** #
#	Utils
# **************************************************************************** #

.PHONY:
gdb_server:
	JLinkGDBServer -device ATSAMC21N18 -if SWD -speed 4000

.PHONY:
clean:
	rm -rf $(BUILD_DIR)
	$(MAKE) -C src/bootloader/ clean
	$(MAKE) -C src/app/ clean
