#include "shell.h"
#include "uart.h"
#include <stdio.h>
#include <string.h>


#define SHELL_PRINTF_BUFFER_SIZE 128

void SHELL_init(void)
{
	UART_init(UART_CHANNEL_SHELL);
}



void SHELL_printf(const char *format, ...)
{
    // char		buffer[SHELL_PRINTF_BUFFER_SIZE];
    // va_list 	args;

    // va_start(args, format);

    // int len = vsnprintf(buffer, sizeof(buffer), format, args);

    // if (len < 0 || len >= (int)sizeof(buffer))
	// {
    //     buffer[sizeof(buffer) - 1] = '\0';
    // }

    // va_end(args);

    // for (char *p = buffer; *p != '\0'; ++p)
	// {
    //     UART_tx_char(UART_CHANNEL_SHELL, *p);
    // }
}