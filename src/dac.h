#ifndef DAC_H
#define DAC_H

#include "common.h"

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void 				DAC_init 				(void);
void 				DAC_write				(uint16_t u16_data);

uint8_t 			DAC_shell_ramp			(uint8_t argc, char ** argv);
uint8_t 			DAC_shell_write			(uint8_t argc, char ** argv);
uint8_t 			DAC_shell_sine			(uint8_t argc, char ** argv);

#endif // DAC_H
