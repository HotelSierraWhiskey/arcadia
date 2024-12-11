#include "sys.h"
#include "io.h"
#include "uart.h"
#include "shell.h"
#include "nvmctrl.h"

/****************************************************************************************************
 *	M A I N
 ****************************************************************************************************/

int main(int argc, char ** argv) 
{
	SYS_init();

	NVMCTRL_init();

	SHELL_init();

	IO_config_pin_direction(IO_PIN_ID_PA27, IO_DIRECTION_OUTPUT);

	while (1)
	{
		SHELL_run();
	}
}
