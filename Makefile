# **************************************************************************** #
#	Compiler Settings & Build Tools
# **************************************************************************** #

ARM_GNU_TOOLCHAIN = /opt/arm_gnu_toolchain
CC = $(ARM_GNU_TOOLCHAIN)/arm-none-eabi-gcc
CCLD = $(ARM_GNU_TOOLCHAIN)/arm-none-eabi-ld
SIZE = $(ARM_GNU_TOOLCHAIN)/arm-none-eabi-size
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
#	Dev Board
# **************************************************************************** #

.PHONY: compile_dev_board
compile_dev_board:
	$(MAKE) -C src/app/dev_board compile_dev_board

.PHONY: compile_dev_board
upload_dev_board:
	$(MAKE) -C src/app/dev_board upload_dev_board

.PHONY: clean_dev_board
clean_dev_board:
	$(MAKE) -C src/app/dev_board clean_dev_board

# **************************************************************************** #
#	Audio Switch
# **************************************************************************** #

.PHONY: compile_audio_switch
compile_audio_switch:
	$(MAKE) -C src/app/audio_switch compile_audio_switch

.PHONY: compile_audio_switch
upload_audio_switch:
	$(MAKE) -C src/app/audio_switch upload_audio_switch

.PHONY: clean_audio_switch
clean_audio_switch:
	$(MAKE) -C src/app/audio_switch clean_audio_switch

# **************************************************************************** #
#	JLink Mux
# **************************************************************************** #

.PHONY: compile_jlink_mux
compile_jlink_mux:
	$(MAKE) -C src/app/jlink_mux compile_jlink_mux

.PHONY: compile_jlink_mux
upload_jlink_mux:
	$(MAKE) -C src/app/jlink_mux upload_jlink_mux

.PHONY: clean_jlink_mux
clean_jlink_mux:
	$(MAKE) -C src/app/jlink_mux clean_jlink_mux

# **************************************************************************** #
#	Utils
# **************************************************************************** #

.PHONY:
gdb_server:
	JLinkGDBServer -device ATSAMC21N18 -if SWD -speed 4000

.PHONY:
clean:
	$(MAKE) clean_audio_switch clean_dev_board clean_jlink_mux