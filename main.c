#include "sys.h"
#include "io.h"
#include "uart.h"
#include "shell.h"

/****************************************************************************************************
 *	M A I N
 ****************************************************************************************************/

int main(int argc, char ** argv) 
{
	SYS_init();

	SHELL_init();

	IO_config_pin_direction(IO_PIN_ID_PA27, IO_DIRECTION_OUTPUT);

	volatile uint32_t i;


	while (1)
	{

		SHELL_printf("clingclong\n\r");

		for (i = 0; i < 4000000; i++)
		{
			continue;
		}
	}
}
