#include "target_port.h"
#include "utils.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define TPORT_LOG_DBG(fmt, ...)   		SHELL_printf("\r%-12s" fmt, "[TPORT]", ##__VA_ARGS__)
#define TPORT_LOG_WARN(fmt, ...)   	SHELL_PRINT_WARNING("\r%-12s" fmt, "[TPORT]", ##__VA_ARGS__)

// LED Pins

#define TARGET_PORT_LED_0_PIN		IO_PIN_ID_PA03
#define TARGET_PORT_LED_1_PIN		IO_PIN_ID_PA02
#define TARGET_PORT_LED_2_PIN		IO_PIN_ID_PC03
#define TARGET_PORT_LED_3_PIN		IO_PIN_ID_PC02
#define TARGET_PORT_LED_4_PIN		IO_PIN_ID_PC01
#define TARGET_PORT_LED_5_PIN		IO_PIN_ID_PC00
#define TARGET_PORT_LED_6_PIN		IO_PIN_ID_PA01
#define TARGET_PORT_LED_7_PIN		IO_PIN_ID_PA00

// Target Port nOE Pins

#define TARGET_PORT_nOE_0_PIN		IO_PIN_ID_PA07
#define TARGET_PORT_nOE_1_PIN		IO_PIN_ID_PA06
#define TARGET_PORT_nOE_2_PIN		IO_PIN_ID_PA05
#define TARGET_PORT_nOE_3_PIN		IO_PIN_ID_PA04
#define TARGET_PORT_nOE_4_PIN		IO_PIN_ID_PB09
#define TARGET_PORT_nOE_5_PIN		IO_PIN_ID_PB08
#define TARGET_PORT_nOE_6_PIN		IO_PIN_ID_PB07
#define TARGET_PORT_nOE_7_PIN		IO_PIN_ID_PB06

typedef enum _target_port_id
{
	TARGET_PORT_ID_0 = 0,
	TARGET_PORT_ID_1,
	TARGET_PORT_ID_2,
	TARGET_PORT_ID_3,
	TARGET_PORT_ID_4,
	TARGET_PORT_ID_5,
	TARGET_PORT_ID_6,
	TARGET_PORT_ID_7,
	//////////
	TARGET_PORT_ID_NUM_PORTS
} target_port_id_t;

static uint8_t u8_selected = 0xFF;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void TARGET_PORT_select_0(void)
{
	if (u8_selected == TARGET_PORT_ID_0)
	{
		return;
	}

	TARGET_PORT_set_leds(IO_PIN_STATE_LOW);
	TARGET_PORT_deselect_all();
	IO_set_pin(TARGET_PORT_nOE_0_PIN, IO_PIN_STATE_LOW);
	IO_set_pin(TARGET_PORT_LED_0_PIN, IO_PIN_STATE_HIGH);
	u8_selected = TARGET_PORT_ID_0;
	TPORT_LOG_DBG("Target Port %u selected\n", u8_selected);
}

void TARGET_PORT_select_1(void)
{
	if (u8_selected == TARGET_PORT_ID_1)
	{
		return;
	}

	TARGET_PORT_set_leds(IO_PIN_STATE_LOW);
	TARGET_PORT_deselect_all();
	IO_set_pin(TARGET_PORT_nOE_1_PIN, IO_PIN_STATE_LOW);
	IO_set_pin(TARGET_PORT_LED_1_PIN, IO_PIN_STATE_HIGH);
	u8_selected = TARGET_PORT_ID_1;
	TPORT_LOG_DBG("Target Port %u selected\n", u8_selected);
}

void TARGET_PORT_select_2(void)
{
	if (u8_selected == TARGET_PORT_ID_2)
	{
		return;
	}

	TARGET_PORT_set_leds(IO_PIN_STATE_LOW);
	TARGET_PORT_deselect_all();
	IO_set_pin(TARGET_PORT_nOE_2_PIN, IO_PIN_STATE_LOW);
	IO_set_pin(TARGET_PORT_LED_2_PIN, IO_PIN_STATE_HIGH);
	u8_selected = TARGET_PORT_ID_2;
	TPORT_LOG_DBG("Target Port %u selected\n", u8_selected);
}

void TARGET_PORT_select_3(void)
{
	if (u8_selected == TARGET_PORT_ID_3)
	{
		return;
	}

	TARGET_PORT_set_leds(IO_PIN_STATE_LOW);
	TARGET_PORT_deselect_all();
	IO_set_pin(TARGET_PORT_nOE_3_PIN, IO_PIN_STATE_LOW);
	IO_set_pin(TARGET_PORT_LED_3_PIN, IO_PIN_STATE_HIGH);
	u8_selected = TARGET_PORT_ID_3;
	TPORT_LOG_DBG("Target Port %u selected\n", u8_selected);
}

void TARGET_PORT_select_4(void)
{
	if (u8_selected == TARGET_PORT_ID_4)
	{
		return;
	}

	TARGET_PORT_set_leds(IO_PIN_STATE_LOW);
	TARGET_PORT_deselect_all();
	IO_set_pin(TARGET_PORT_nOE_4_PIN, IO_PIN_STATE_LOW);
	IO_set_pin(TARGET_PORT_LED_4_PIN, IO_PIN_STATE_HIGH);
	u8_selected = TARGET_PORT_ID_4;
	TPORT_LOG_DBG("Target Port %u selected\n", u8_selected);
}

void TARGET_PORT_select_5(void)
{
	if (u8_selected == TARGET_PORT_ID_5)
	{
		return;
	}

	TARGET_PORT_set_leds(IO_PIN_STATE_LOW);
	TARGET_PORT_deselect_all();
	IO_set_pin(TARGET_PORT_nOE_5_PIN, IO_PIN_STATE_LOW);
	IO_set_pin(TARGET_PORT_LED_5_PIN, IO_PIN_STATE_HIGH);
	u8_selected = TARGET_PORT_ID_5;
	TPORT_LOG_DBG("Target Port %u selected\n", u8_selected);
}

void TARGET_PORT_select_6(void)
{
	if (u8_selected == TARGET_PORT_ID_6)
	{
		return;
	}

	TARGET_PORT_set_leds(IO_PIN_STATE_LOW);
	TARGET_PORT_deselect_all();
	IO_set_pin(TARGET_PORT_nOE_6_PIN, IO_PIN_STATE_LOW);
	IO_set_pin(TARGET_PORT_LED_6_PIN, IO_PIN_STATE_HIGH);
	u8_selected = TARGET_PORT_ID_6;
	TPORT_LOG_DBG("Target Port %u selected\n", u8_selected);
}

void TARGET_PORT_select_7(void)
{
	if (u8_selected == TARGET_PORT_ID_7)
	{
		return;
	}

	TARGET_PORT_set_leds(IO_PIN_STATE_LOW);
	TARGET_PORT_deselect_all();
	IO_set_pin(TARGET_PORT_nOE_7_PIN, IO_PIN_STATE_LOW);
	IO_set_pin(TARGET_PORT_LED_7_PIN, IO_PIN_STATE_HIGH);
	u8_selected = TARGET_PORT_ID_7;
	TPORT_LOG_DBG("Target Port %u selected\n", u8_selected);
}

void TARGET_PORT_set_leds(IO_pin_state_t state)
{
	IO_set_pin(TARGET_PORT_LED_0_PIN, state);
	IO_set_pin(TARGET_PORT_LED_1_PIN, state);
	IO_set_pin(TARGET_PORT_LED_2_PIN, state);
	IO_set_pin(TARGET_PORT_LED_3_PIN, state);
	IO_set_pin(TARGET_PORT_LED_4_PIN, state);
	IO_set_pin(TARGET_PORT_LED_5_PIN, state);
	IO_set_pin(TARGET_PORT_LED_6_PIN, state);
	IO_set_pin(TARGET_PORT_LED_7_PIN, state);
}

void TARGET_PORT_deselect_all(void)
{
	IO_set_pin(TARGET_PORT_nOE_0_PIN, IO_PIN_STATE_HIGH);
	IO_set_pin(TARGET_PORT_nOE_1_PIN, IO_PIN_STATE_HIGH);
	IO_set_pin(TARGET_PORT_nOE_2_PIN, IO_PIN_STATE_HIGH);
	IO_set_pin(TARGET_PORT_nOE_3_PIN, IO_PIN_STATE_HIGH);
	IO_set_pin(TARGET_PORT_nOE_4_PIN, IO_PIN_STATE_HIGH);
	IO_set_pin(TARGET_PORT_nOE_5_PIN, IO_PIN_STATE_HIGH);
	IO_set_pin(TARGET_PORT_nOE_6_PIN, IO_PIN_STATE_HIGH);
	IO_set_pin(TARGET_PORT_nOE_7_PIN, IO_PIN_STATE_HIGH);
}

/****************************************************************************************************
 *	Shell utility
 *
 * 	Selects a target port
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t	TARGET_PORT_select(uint8_t argc, char ** argv)
{
	uint32_t 		u32_port;
	bool 			b_res = true;

	if (argc == 1)
	{
		if (UTILS_string_to_u32(argv[0], &u32_port))
		{
			switch(u32_port)
			{
				case TARGET_PORT_ID_0:
					TARGET_PORT_select_0();
					break;
				case TARGET_PORT_ID_1:
					TARGET_PORT_select_1();
					break;
				case TARGET_PORT_ID_2:
					TARGET_PORT_select_2();
					break;
				case TARGET_PORT_ID_3:
					TARGET_PORT_select_3();
					break;
				case TARGET_PORT_ID_4:
					TARGET_PORT_select_4();
					break;
				case TARGET_PORT_ID_5:
					TARGET_PORT_select_5();
					break;
				case TARGET_PORT_ID_6:
					TARGET_PORT_select_6();
					break;
				case TARGET_PORT_ID_7:
					TARGET_PORT_select_7();
					break;
				default:
					b_res = false;
			}
		}
	}
	else
	{
		b_res = false;
	}

	if (!b_res)
	{
		SHELL_printf("Usage: port select <port number>\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

/****************************************************************************************************
 *	Shell utility
 *
 * 	Displays target port info
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t	TARGET_PORT_info(uint8_t argc, char ** argv)
{
	if (argc == 0)
	{
		SHELL_SEPARATOR();

		for (uint8_t i = 0; i < TARGET_PORT_ID_NUM_PORTS; i++)
		{
			SHELL_printf("Port %u:\t%s\n" , i, i == u8_selected ? "[X]" : "[ ]");
		}

		SHELL_SEPARATOR();
	}
	else
	{
		SHELL_printf("Usage: port info\n");
	}

	return SHELL_COMMAND_SUCCESS;
}
