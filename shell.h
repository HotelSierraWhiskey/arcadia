#ifndef SHELL_H
#define SHELL_H

#include "common.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define SHELL_COMMAND_SUCCESS			(0)
#define	SHELL_SEPARATOR() 				SHELL_printf("----------------------------------------\r\n")
#define SHELL_PROMPT					"> "

#define SHELL_COLOR_RESET       		"\033[0m"
#define SHELL_COLOR_BLACK       		"\033[30m"
#define SHELL_COLOR_RED         		"\033[31m"
#define SHELL_COLOR_GREEN       		"\033[32m"
#define SHELL_COLOR_YELLOW      		"\033[33m"
#define SHELL_COLOR_BLUE        		"\033[34m"
#define SHELL_COLOR_MAGENTA     		"\033[35m"
#define SHELL_COLOR_CYAN        		"\033[36m"
#define SHELL_COLOR_WHITE       		"\033[37m"
#define SHELL_COLOR_BOLD        		"\033[1m"
#define SHELL_COLOR_UNDERLINE   		"\033[4m"

#define SHELL_COLOR_BRIGHT_BLACK  		"\033[90m"
#define SHELL_COLOR_BRIGHT_RED    		"\033[91m"
#define SHELL_COLOR_BRIGHT_GREEN  		"\033[92m"
#define SHELL_COLOR_BRIGHT_YELLOW 		"\033[93m"
#define SHELL_COLOR_BRIGHT_BLUE   		"\033[94m"
#define SHELL_COLOR_BRIGHT_MAGENTA 		"\033[95m"
#define SHELL_COLOR_BRIGHT_CYAN   		"\033[96m"
#define SHELL_COLOR_BRIGHT_WHITE  		"\033[97m"

#define SHELL_COLOR_VAPORWAVE_PINK    	"\033[38;5;213m"
#define SHELL_COLOR_VAPORWAVE_PURPLE  	"\033[38;5;135m"
#define SHELL_COLOR_VAPORWAVE_AQUA    	"\033[38;5;51m"
#define SHELL_COLOR_VAPORWAVE_PEACH   	"\033[38;5;223m"
#define SHELL_COLOR_VAPORWAVE_MINT    	"\033[38;5;120m"
#define SHELL_COLOR_VAPORWAVE_LILAC   	"\033[38;5;171m"
#define SHELL_COLOR_VAPORWAVE_BLUE    	"\033[38;5;33m"
#define SHELL_COLOR_VAPORWAVE_LAVENDER 	"\033[38;5;141m"

#define SHELL_PRINT_COLOR(color, format, ...) \
	do { \
		SHELL_printf(color format SHELL_COLOR_RESET, ##__VA_ARGS__); \
		if (format[strlen(format) - 1] != '\n') \
		{ \
			SHELL_printf("\r\n"); \
		} \
		SHELL_printf("%s", SHELL_PROMPT); \
	} while(0)

#define SHELL_PRINT_ERROR(error, ...) \
    SHELL_PRINT_COLOR(SHELL_COLOR_RED, error, ##__VA_ARGS__)

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/


void	SHELL_init				(void);
void 	SHELL_task				(void * p_params);
void 	SHELL_printf			(const char *format, ...);
void 	SHELL_display_banner	(void);


#endif // SHELL_H
