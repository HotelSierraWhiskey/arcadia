# **************************************************************************** #
#	Compiler Settings & Build Tools
# **************************************************************************** #
CC      := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
SIZE    := arm-none-eabi-size

# **************************************************************************** #
#	Project
# **************************************************************************** #
TARGET  := bootloader
BUILD   := build

# **************************************************************************** #
#	MCU/ Arch
# **************************************************************************** #
CPU     := cortex-m0plus
ARCH    := -mcpu=$(CPU) -mthumb

# **************************************************************************** #
#	Bootloader Flags
# **************************************************************************** #
BOOTLOADER_FLAGS := -DBOOTLOADER

# **************************************************************************** #
#	Includes
# **************************************************************************** #
INC :=	-I. \
		-I../ \
		-I../../lib/CMSIS_5/CMSIS/Core/Include \
		-I../../lib/samc21/include \
		-I/usr/local/arm-gnu-toolchain-14.3.Rel1-x86_64-arm-none-eabi/include


# **************************************************************************** #
#	Compiler Flags
# **************************************************************************** #
CFLAGS  := $(ARCH) \
			-std=gnu2x \
			-ffunction-sections \
			-fdata-sections \
			-nostartfiles \
			--specs=nosys.specs \
			-Wno-switch \
			-Wno-main \
			-Wl,--print-memory-usage \
			-lm \
			$(BOOTLOADER_FLAGS) \
			$(INC)

# **************************************************************************** #
#	Linker Flags
# **************************************************************************** #
LDFLAGS := $(ARCH) \
		   -T linkerscript.ld \
		   -Wl,--gc-sections \
		   -Wl,-Map=$(BUILD)/$(TARGET).map

# **************************************************************************** #
#	Sources
# **************************************************************************** #
SRCS := main.c \
		../../nvic.c \
		../../io.c \
		../../sercom.c \
		../../uart.c
OBJS := $(addprefix $(BUILD)/, $(SRCS:.c=.o))

# **************************************************************************** #
#	Rules
# **************************************************************************** #
all: $(BUILD)/$(TARGET).elf $(BUILD)/$(TARGET).hex $(BUILD)/$(TARGET).bin

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.o: %.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/$(TARGET).elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@
	$(SIZE) $@

$(BUILD)/$(TARGET).hex: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf $(BUILD)

.PHONY: all clean