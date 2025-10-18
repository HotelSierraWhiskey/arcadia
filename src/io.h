#ifndef IO_H
#define IO_H

#include "common.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

/**
 *	Pin identifiers
 *
 * 	Used as indices in the application's pin map
 */
typedef enum _IO_pin_id
{
	// Port A
	IO_PIN_ID_PA00 = 0,
	IO_PIN_ID_PA01,
	IO_PIN_ID_PA02,
	IO_PIN_ID_PA03,
	IO_PIN_ID_PA04,
	IO_PIN_ID_PA05,
	IO_PIN_ID_PA06,
	IO_PIN_ID_PA07,
	IO_PIN_ID_PA08,
	IO_PIN_ID_PA09,
	IO_PIN_ID_PA10,
	IO_PIN_ID_PA11,
	IO_PIN_ID_PA12,
	IO_PIN_ID_PA13,
	IO_PIN_ID_PA14,
	IO_PIN_ID_PA15,
	IO_PIN_ID_PA16,
	IO_PIN_ID_PA17,
	IO_PIN_ID_PA18,
	IO_PIN_ID_PA19,
	IO_PIN_ID_PA20,
	IO_PIN_ID_PA21,
	IO_PIN_ID_PA22,
	IO_PIN_ID_PA23,
	IO_PIN_ID_PA24,
	IO_PIN_ID_PA25,

	// no PA26

	IO_PIN_ID_PA27,
	IO_PIN_ID_PA28,
	IO_PIN_ID_PA30,
	IO_PIN_ID_PA31,
	
	// Port B
	IO_PIN_ID_PB00,
	IO_PIN_ID_PB01,
	IO_PIN_ID_PB02,
	IO_PIN_ID_PB03,
	IO_PIN_ID_PB04,
	IO_PIN_ID_PB05,
	IO_PIN_ID_PB06,
	IO_PIN_ID_PB07,
	IO_PIN_ID_PB08,
	IO_PIN_ID_PB09,
	IO_PIN_ID_PB10,
	IO_PIN_ID_PB11,
	IO_PIN_ID_PB12,
	IO_PIN_ID_PB13,
	IO_PIN_ID_PB14,
	IO_PIN_ID_PB15,
	IO_PIN_ID_PB16,
	IO_PIN_ID_PB17,

#ifdef MCU_SAMC21N18A
	IO_PIN_ID_PB18,
	IO_PIN_ID_PB19,
	IO_PIN_ID_PB20,
	IO_PIN_ID_PB21,
#endif // MCU_SAMC21E18A

	IO_PIN_ID_PB22,
	IO_PIN_ID_PB23,

#ifdef MCU_SAMC21N18A
	IO_PIN_ID_PB24,
	IO_PIN_ID_PB25,
#endif // MCU_SAMC21E18A

	// no PB26
	// no PB27
	// no PB28

	IO_PIN_ID_PB30,
	IO_PIN_ID_PB31,

#ifdef MCU_SAMC21N18A
	// Port C
	IO_PIN_ID_PC00,
	IO_PIN_ID_PC01,
	IO_PIN_ID_PC02,
	IO_PIN_ID_PC03,

	// no PC04,

	IO_PIN_ID_PC05,
	IO_PIN_ID_PC06,
	IO_PIN_ID_PC07,
	IO_PIN_ID_PC08,
	IO_PIN_ID_PC09,
	IO_PIN_ID_PC10,
	IO_PIN_ID_PC11,
	IO_PIN_ID_PC12,
	IO_PIN_ID_PC13,
	IO_PIN_ID_PC14,
	IO_PIN_ID_PC15,
	IO_PIN_ID_PC16,
	IO_PIN_ID_PC17,
	IO_PIN_ID_PC18,
	IO_PIN_ID_PC19,
	IO_PIN_ID_PC20,
	IO_PIN_ID_PC21,

	// no PC22,
	// no PC23,

	IO_PIN_ID_PC24,
	IO_PIN_ID_PC25,
	IO_PIN_ID_PC27,
	IO_PIN_ID_PC28,
#endif // MCU_SAMC21N18A

	//////////
	IO_PIN_ID_NUM_PINS
} IO_pin_id_t;

/**
 *	Peripheral functions
 */
typedef enum _IO_peripheral_function
{
	IO_PERIPHERAL_FUNCTION_A = 0,
	IO_PERIPHERAL_FUNCTION_B,
	IO_PERIPHERAL_FUNCTION_C,
	IO_PERIPHERAL_FUNCTION_D,
	IO_PERIPHERAL_FUNCTION_E,
	IO_PERIPHERAL_FUNCTION_F,
	IO_PERIPHERAL_FUNCTION_G,
	IO_PERIPHERAL_FUNCTION_H,
	IO_PERIPHERAL_FUNCTION_I,
	//////////
	IO_PERIPHERAL_FUNCTION_NUM_FUNCTIONS,
} IO_peripheral_function_t;

/**
 *	Pin direction configurations
 *	
 *	(input/ output)
 */
typedef enum _IO_pin_direction
{
	IO_DIRECTION_INPUT = 0,
	IO_DIRECTION_OUTPUT,
	//////////
	IO_DIRECTION_NUM_DIRECTIONS
} IO_pin_direction_t;

/**
 *	Pin IO states
 *	
 *	(high/ low)
 */
typedef enum _IO_pin_state
{
	IO_PIN_STATE_LOW = 0,
	IO_PIN_STATE_HIGH,
	//////////
	IO_PIN_STATE_NUM_STATES
} IO_pin_state_t;

/**
 *	Pin types
 */
typedef enum _IO_pin_type
{
	IO_PIN_TYPE_UNUSED = 0,
	IO_PIN_TYPE_INPUT,
	IO_PIN_TYPE_OUTPUT,
	IO_PIN_TYPE_PERIPHERAL,
	//////////
	IO_PIN_TYPE_NUM_TYPES
} IO_pin_type_t;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void 			IO_init									(void);
void			IO_enable_peripheral_function_for_pin	(IO_pin_id_t pin_id, IO_peripheral_function_t peripheral_function);
void			IO_disable_peripheral_function_for_pin	(IO_pin_id_t pin_id, IO_peripheral_function_t peripheral_function);
void			IO_config_pin_direction					(IO_pin_id_t pin_id, IO_pin_direction_t direction);
void			IO_set_pin								(IO_pin_id_t pin_id, IO_pin_state_t state);
IO_pin_state_t	IO_read_pin								(IO_pin_id_t pin_id);
void			IO_enable_pullup						(IO_pin_id_t pin_id);
void			IO_disable_pullup						(IO_pin_id_t pin_id);
void			IO_enable_strong_drive_strength			(IO_pin_id_t pin_id);
void			IO_disable_strong_drive_strength		(IO_pin_id_t pin_id);
const char *	IO_get_pin_name							(IO_pin_id_t pin_id);
const char * 	IO_get_pin_string						(IO_pin_id_t pin_id);

/****************************************************************************************************
 *	S H E L L   F U N C T I O N S
 ****************************************************************************************************/

uint8_t 		IO_shell_map							(uint8_t argc, char ** argv);
uint8_t			IO_shell_set							(uint8_t argc, char ** argv);

#endif // IO_H
