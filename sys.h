#ifndef SYS_H
#define SYS_H

#define SYS_CLOCK_FREQ	(48000000UL)

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void 		SYS_init 	( void );

uint8_t 	SYS_shell_info	(uint8_t argc, char ** argv);

#endif // SYS_H
