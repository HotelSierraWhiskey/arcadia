#ifndef UART_H
#define UART_H


#include "includes.h"


typedef struct 
{

} UART_config_t;


void UART_init		(void);
void UART_tx_char	(const char data);


#endif