#ifndef IO_MAP
#define IO_MAP

#include "includes.h"

#define IO_PIN_NAME_MAX_LEN 64

typedef enum 
{
	IO_PIN_PA00 = 0,
	IO_PIN_PA01,
	IO_PIN_PA02,
	IO_PIN_PA03,
	IO_PIN_PA04,
	IO_PIN_PA05,
	IO_PIN_PA06,
	IO_PIN_PA07,
	IO_PIN_PA08,
	IO_PIN_PA09,
	IO_PIN_PA10,
	IO_PIN_PA11,
	IO_PIN_PA14,
	IO_PIN_PA15,
	IO_PIN_PA16,
	IO_PIN_PA17,
	IO_PIN_PA18,
	IO_PIN_PA19,
	IO_PIN_PA22,
	IO_PIN_PA23,
	IO_PIN_PA24,
	IO_PIN_PA25,
	IO_PIN_PA27,
	IO_PIN_PA28,
	IO_PIN_PA30,
	IO_PIN_PA31,
	IO_PIN_RESET,
	//
	IO_PIN_NUM_PINS
} IO_pin_designation_t;

typedef enum 
{
	IO_PORT_A,
	//
	IO_NUM_PORTS
} IO_port_t;

typedef enum 
{
	IO_DIRECTION_INPUT,
	IO_DIRECTION_OUTPUT,
	IO_DIRECTION_OPEN_DRAIN,
	//
	IO_NUM_DIRECTIONS
} IO_direction_t;

typedef enum 
{
	IO_PERIPHERAL_NOT_A_PERIPHERAL,
	IO_PERIPHERAL_SERCOM_UART,
	//
	IO_NUM_PERIPHERALS
} IO_peripheral_id_t;

typedef enum
{
	IO_PERIPHERAL_FUNCTION_A,
	IO_PERIPHERAL_FUNCTION_B,
	IO_PERIPHERAL_FUNCTION_C,
	IO_PERIPHERAL_FUNCTION_D,
	IO_PERIPHERAL_FUNCTION_E,
	IO_PERIPHERAL_FUNCTION_F,
	IO_PERIPHERAL_FUNCTION_G,
	IO_PERIPHERAL_FUNCTION_H,
	//
	IO_NUM_PERIPHERAL_FUNCTIONS
} IO_peripheral_function_t;

/// Contains configuration information relevant to a single pin.
typedef struct _IO_pin_config
{
	const char 			kpc_name[IO_PIN_NAME_MAX_LEN];
	IO_port_t			port;
	const uint8_t		ku8_gpio_pin_number;
	IO_direction_t		direction;
	IO_peripheral_id_t 	peripheral;
} IO_pin_config_t;

void 					IO_init				(void);
void 					IO_pin_gpio_init	(const IO_pin_designation_t pin);
void 					IO_pin_assert		(const IO_pin_designation_t pin);
void 					IO_pin_deassert		(const IO_pin_designation_t pin);
void 					IO_pin_toggle		(const IO_pin_designation_t pin);
const IO_pin_config_t *	IO_get_pin_config	(const IO_pin_designation_t pin);
void 					IO_pin_pmux_enable	(const IO_pin_designation_t pin, const IO_peripheral_function_t function);
void 					IO_pin_pmux_disable	(const IO_pin_designation_t pin, const IO_peripheral_function_t function);


#endif // IO_MAP