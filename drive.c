#include "drive.h"
#include "common.h"
#include "shell.h"
#include "arcadia.h"


void DRIVE_task(void * p_params)
{
	(void)p_params;

	uint32_t data = 0;

	while (1)
	{
		if (ARCADIA_receive(ARCADIA_TASK_ID_DRIVE, (void *)&data, 1000))
		{
			SHELL_printf("Received: %u\r\n", data);
		}

		vPortYield();
	}
}
