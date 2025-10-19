#include "common.h"
#include "io.h"

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void		TARGET_PORT_set_leds		(IO_pin_state_t state);
void		TARGET_PORT_deselect_all	(void);

void		TARGET_PORT_select_0		(void);
void		TARGET_PORT_select_1		(void);
void		TARGET_PORT_select_2		(void);
void		TARGET_PORT_select_3		(void);
void		TARGET_PORT_select_4		(void);
void		TARGET_PORT_select_5		(void);
void		TARGET_PORT_select_6		(void);
void		TARGET_PORT_select_7		(void);

uint8_t		TARGET_PORT_select			(uint8_t argc, char ** argv);
uint8_t		TARGET_PORT_info			(uint8_t argc, char ** argv);
