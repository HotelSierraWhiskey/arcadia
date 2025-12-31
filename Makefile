# **************************************************************************** #
#	Compiler Settings & Build Tools
# **************************************************************************** #

ARM_GNU_TOOLCHAIN = /opt/arm_gnu_toolchain
CC = $(ARM_GNU_TOOLCHAIN)/arm-none-eabi-gcc
CCLD = $(ARM_GNU_TOOLCHAIN)/arm-none-eabi-ld
SIZE = $(ARM_GNU_TOOLCHAIN)/arm-none-eabi-size
OBJCOPY := $(ARM_GNU_TOOLCHAIN)/arm-none-eabi-objcopy
SREC_CAT := srec_cat
JLINK = JLinkExe

COMMON_FLAGS = \
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

export CC CCLD SIZE JLINK COMMON_FLAGS

# **************************************************************************** #
#	Paths
# **************************************************************************** #

SRC				:= src
BOOTLOADER_DIR	:= $(SRC)/bootloader
APP_DIR			:= $(SRC)/app/dev_board

BOOTLOADER_HEX	:= $(BOOTLOADER_DIR)/build/bootloader.hex
APP_ELF			:= $(APP_DIR)/build/app_dev_board.elf
APP_HEX			:= $(APP_DIR)/build/app_dev_board.hex
COMBINED_HEX	:= $(APP_DIR)/build/combined.hex

# **************************************************************************** #
#	Bootloader
# **************************************************************************** #

.PHONY: compile_bootloader
compile_bootloader:
	$(MAKE) -C $(BOOTLOADER_DIR) clean
	$(MAKE) -C $(BOOTLOADER_DIR)

# **************************************************************************** #
#	Dev Board
# **************************************************************************** #

.PHONY: compile_dev_board
compile_dev_board:
	$(MAKE) -C src/app/dev_board compile_dev_board

.PHONY: clean_dev_board
clean_dev_board:
	$(MAKE) -C src/app/dev_board clean_dev_board

.PHONY: compile_dev_board
upload_dev_board:
	$(MAKE) -C src/app/dev_board upload_dev_board

.PHONY: app_hex
app_hex: $(APP_HEX)

$(APP_HEX): $(APP_ELF)
	$(OBJCOPY) -O ihex $< $@

# **************************************************************************** #
#	Combine hex
# **************************************************************************** #

.PHONY: combine_hex
combine_hex: $(COMBINED_HEX)

$(COMBINED_HEX): $(BOOTLOADER_HEX) $(APP_HEX)
	$(SREC_CAT) $(BOOTLOADER_HEX) -Intel $(APP_HEX) -Intel -o $@ -Intel

# **************************************************************************** #
#	Unified
# **************************************************************************** #

.PHONY: upload_unified
upload_unified: compile_bootloader clean_dev_board compile_dev_board app_hex combine_hex
	$(MAKE) -C $(APP_DIR) upload_dev_board

# **************************************************************************** #
#	Utils
# **************************************************************************** #

.PHONY:
gdb_server:
	JLinkGDBServer -device ATSAMC21N18 -if SWD -speed 4000


# FIXME:
# some bootloader deps are being built in the source tree
# they should be built in src/bootloader/build
.PHONY:
clean:
	rm -f $(SRC)/*.o
	$(MAKE) -C $(BOOTLOADER_DIR) clean
	$(MAKE) clean_dev_board
