#include "io.h"

#define IO_NOT_A_GPIO 0xFF

IO_pin_config_t IO_pin_map[IO_PIN_NUM_PINS] =
{
	[IO_PIN_PA00] =
	{
		.kpc_name = "BOOT_BUTTON",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = 0,
		.direction = IO_DIRECTION_INPUT,
		.peripheral = IO_PERIPHERAL_NOT_A_PERIPHERAL
	},
	[IO_PIN_PA01] =
	{
		.kpc_name = "DEBUG_BUTTON",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = 1,
		.direction = IO_DIRECTION_INPUT,
		.peripheral = IO_PERIPHERAL_NOT_A_PERIPHERAL
	},
	[IO_PIN_PA02] =
	{
		.kpc_name = "DEBUG_LED_1",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = 2,
		.direction = IO_DIRECTION_OUTPUT,
		.peripheral = IO_PERIPHERAL_NOT_A_PERIPHERAL
	},
	[IO_PIN_PA03] =
	{
		.kpc_name = "DEBUG_LED_2",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = 3,
		.direction = IO_DIRECTION_OUTPUT,
		.peripheral = IO_PERIPHERAL_NOT_A_PERIPHERAL
	},
	[IO_PIN_PA04] =
	{
		.kpc_name = "DEBUG_LED_3",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = 4,
		.direction = IO_DIRECTION_OUTPUT,
		.peripheral = IO_PERIPHERAL_NOT_A_PERIPHERAL
	},
	[IO_PIN_PA05] =
	{
		.kpc_name = "DEBUG_LED_4",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = 5,
		.direction = IO_DIRECTION_OUTPUT,
		.peripheral = IO_PERIPHERAL_NOT_A_PERIPHERAL
	},
	[IO_PIN_PA06] =
	{
		.kpc_name = "UNUSED",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = 6,
		.direction = IO_DIRECTION_OUTPUT,
		.peripheral = IO_PERIPHERAL_NOT_A_PERIPHERAL
	},
	[IO_PIN_PA07] =
	{
		.kpc_name = "UNUSED",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = 7,
		// .direction = IO_DIRECTION_OUTPUT,
		.direction = IO_DIRECTION_INPUT,
		.peripheral = IO_PERIPHERAL_NOT_A_PERIPHERAL
	},
	[IO_PIN_PA08] =
	{
		.kpc_name = "UNUSED",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = 8,
		.direction = IO_DIRECTION_OUTPUT,
		.peripheral = IO_PERIPHERAL_SERCOM_UART
	},
	[IO_PIN_PA09] =
	{
		.kpc_name = "UNUSED",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = 9,
		.direction = IO_DIRECTION_OUTPUT,
		.peripheral = IO_PERIPHERAL_SERCOM_UART
	},
	[IO_PIN_PA10] =
	{
		.kpc_name = "APP_UART_TX",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = 10,
		.direction = IO_DIRECTION_OUTPUT,
		.peripheral = IO_PERIPHERAL_SERCOM_UART
	},
	[IO_PIN_PA11] =
	{
		.kpc_name = "APP_UART_RX",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = 11,
		// .direction = IO_DIRECTION_INPUT,
		.direction = IO_DIRECTION_OUTPUT,
		.peripheral = IO_PERIPHERAL_SERCOM_UART
	},
	[IO_PIN_PA14] =
	{
		.kpc_name = "EXT_FLASH_CS",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = 14,
		.direction = IO_DIRECTION_INPUT,
		.peripheral = IO_PERIPHERAL_NOT_A_PERIPHERAL
	},
	[IO_PIN_PA15] =
	{
		.kpc_name = "EXT_FLASH_SCK",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = 15,
		.direction = IO_DIRECTION_INPUT,
		.peripheral = IO_PERIPHERAL_NOT_A_PERIPHERAL
	},
	[IO_PIN_PA16] =
	{
		.kpc_name = "EXT_FLASH_DI",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = 16,
		.direction = IO_DIRECTION_INPUT,
		.peripheral = IO_PERIPHERAL_NOT_A_PERIPHERAL
	},
	[IO_PIN_PA17] =
	{
		.kpc_name = "EXT_FLASH_DO",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = 17,
		.direction = IO_DIRECTION_INPUT,
		.peripheral = IO_PERIPHERAL_NOT_A_PERIPHERAL
	},
	[IO_PIN_PA18] =
	{
		.kpc_name = "EXT_FLASH_WP",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = 18,
		.direction = IO_DIRECTION_INPUT,
		.peripheral = IO_PERIPHERAL_NOT_A_PERIPHERAL
	},
	[IO_PIN_PA19] =
	{
		.kpc_name = "EXT_FLASH_HOLD",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = 19,
		.direction = IO_DIRECTION_INPUT,
		.peripheral = IO_PERIPHERAL_NOT_A_PERIPHERAL
	},
	[IO_PIN_PA22] =
	{
		.kpc_name = "UNUSED",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = 22,
		.direction = IO_DIRECTION_OUTPUT,
		.peripheral = IO_PERIPHERAL_NOT_A_PERIPHERAL
	},
	[IO_PIN_PA23] =
	{
		.kpc_name = "UNUSED",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = 23,
		.direction = IO_DIRECTION_OUTPUT,
		.peripheral = IO_PERIPHERAL_NOT_A_PERIPHERAL
	},
	[IO_PIN_PA24] =
	{
		.kpc_name = "DEBUG_UART_TX",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = 24,
		.direction = IO_DIRECTION_OUTPUT,
		.peripheral = IO_PERIPHERAL_SERCOM_UART
	},
	[IO_PIN_PA25] =
	{
		.kpc_name = "DEBUG_UART_RX",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = 25,
		.direction = IO_DIRECTION_INPUT,
		// .direction = IO_DIRECTION_OUTPUT,
		.peripheral = IO_PERIPHERAL_SERCOM_UART
	},
	[IO_PIN_PA27] =
	{
		.kpc_name = "UNUSED",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = 27,
		.direction = IO_DIRECTION_OUTPUT,
		.peripheral = IO_PERIPHERAL_NOT_A_PERIPHERAL
	},
	[IO_PIN_PA28] =
	{
		.kpc_name = "UNUSED",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = 28,
		.direction = IO_DIRECTION_OUTPUT,
		.peripheral = IO_PERIPHERAL_NOT_A_PERIPHERAL
	},
	[IO_PIN_PA30] =
	{
		.kpc_name = "SWD_CLK",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = 30,
		.direction = IO_DIRECTION_OUTPUT,
		.peripheral = IO_PERIPHERAL_NOT_A_PERIPHERAL
	},
	[IO_PIN_PA31] =
	{
		.kpc_name = "SWD_DATA",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = 31,
		.direction = IO_DIRECTION_OPEN_DRAIN,
		.peripheral = IO_PERIPHERAL_NOT_A_PERIPHERAL
	},
	[IO_PIN_RESET] =
	{
		.kpc_name = "RESET_BUTTON",
		.port = IO_PORT_A,
		.ku8_gpio_pin_number = IO_NOT_A_GPIO,
		.direction = IO_DIRECTION_OPEN_DRAIN,
		.peripheral = IO_PERIPHERAL_NOT_A_PERIPHERAL
	},
};

void IO_init(void)
{
	PM_REGS->PM_APBBMASK |= PM_APBBMASK_PORT(1);
}

void IO_pin_gpio_init(const IO_pin_designation_t pin)
{
	const uint8_t ku8_port = IO_pin_map[pin].port;
	const uint8_t ku8_shift = IO_pin_map[pin].ku8_gpio_pin_number;

	if (IO_pin_map[pin].direction == IO_DIRECTION_OUTPUT)
	{
		PORT_REGS->GROUP[ku8_port].PORT_PINCFG[ku8_shift] |= PORT_PINCFG_INEN(0);
        PORT_REGS->GROUP[ku8_port].PORT_DIRSET = (1 << ku8_shift);
	}
	else
	{
		PORT_REGS->GROUP[ku8_port].PORT_PINCFG[ku8_shift] |= PORT_PINCFG_INEN(1);
        PORT_REGS->GROUP[ku8_port].PORT_DIRCLR = (1 << ku8_shift);
	}
}

void IO_pin_assert(const IO_pin_designation_t pin)
{
	const uint8_t ku8_port = IO_pin_map[pin].port;	
	const uint8_t ku8_shift = IO_pin_map[pin].ku8_gpio_pin_number;

	PORT_REGS->GROUP[ku8_port].PORT_OUTSET = (1 << ku8_shift);
}

void IO_pin_deassert(const IO_pin_designation_t pin)
{
	const uint8_t ku8_port = IO_pin_map[pin].port;	
	const uint8_t ku8_shift = IO_pin_map[pin].ku8_gpio_pin_number;
	
	PORT_REGS->GROUP[ku8_port].PORT_OUTCLR = (1 << ku8_shift);
}

void IO_pin_toggle(const IO_pin_designation_t pin)
{
	const uint8_t ku8_port = IO_pin_map[pin].port;	
	const uint8_t ku8_shift = IO_pin_map[pin].ku8_gpio_pin_number;

	PORT_REGS->GROUP[ku8_port].PORT_OUTTGL = (1 << ku8_shift);
}

const IO_pin_config_t *IO_get_pin_config(const IO_pin_designation_t pin)
{
	return (const IO_pin_config_t *)&IO_pin_map[pin];
}

bool IO_pin_read(const IO_pin_designation_t pin)
{
	const uint8_t ku8_port = IO_pin_map[pin].port;	
	const uint8_t ku8_shift = IO_pin_map[pin].ku8_gpio_pin_number;

	return PORT_REGS->GROUP[ku8_port].PORT_IN & (1 << ku8_shift);
}

void IO_pin_pmux_enable(const IO_pin_designation_t pin, const IO_peripheral_function_t function)
{
	const IO_pin_config_t *config = IO_get_pin_config(pin);
	const uint8_t ku8_gpio = config->ku8_gpio_pin_number;

	PORT_REGS->GROUP[config->port].PORT_PINCFG[ku8_gpio] |= PORT_PINCFG_PMUXEN(1);

	if (ku8_gpio % 2 != 0)
	{
		PORT_REGS->GROUP[config->port].PORT_PMUX[ku8_gpio >> 1] |= PORT_PMUX_PMUXO(function);
	}
	else
	{
		PORT_REGS->GROUP[config->port].PORT_PMUX[ku8_gpio >> 1] |= PORT_PMUX_PMUXE(function);
	}
}

void IO_pin_pmux_disable(const IO_pin_designation_t pin, const IO_peripheral_function_t function)
{
	const IO_pin_config_t *config = IO_get_pin_config(pin);
	const uint8_t ku8_gpio = config->ku8_gpio_pin_number;

	PORT_REGS->GROUP[config->port].PORT_PINCFG[ku8_gpio] |= PORT_PINCFG_PMUXEN(0);

	if (ku8_gpio % 2 != 0)
	{
		PORT_REGS->GROUP[config->port].PORT_PMUX[ku8_gpio >> 1] &= ~PORT_PMUX_PMUXO(function);
	}
	else
	{
		PORT_REGS->GROUP[config->port].PORT_PMUX[ku8_gpio >> 1] &= ~PORT_PMUX_PMUXE(function);
	}
}