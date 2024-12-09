#include "sys.h"
#include "io.h"
#include "uart.h"
#include "shell.h"

#include <math.h>
#include <string.h>
#include <stdio.h>


// int _write(int file, char *ptr, int len) {
//     for (int i = 0; i < len; i++) {
//             UART_tx_char(UART_CHANNEL_SHELL, ptr[i]);
//         }
//     return len;
// }

int puts(const char *str)
{
    if (!str) {
        return -1;
    }

    while (*str) {
        UART_tx_char(UART_CHANNEL_SHELL, *str++);
    }

    UART_tx_char(UART_CHANNEL_SHELL, '\n');

    return 0;
}

int putchar(int c)
{
    UART_tx_char(UART_CHANNEL_SHELL, c);
    return c;
}

int main(int argc, char ** argv) 
{
	SYS_init();

	SHELL_init();

	volatile uint32_t i;

	while (1)
	{

		printf("Anniversary\n");

		for (i = 0; i < 4000000; i++)
		{
			continue;
		}
	}
}
