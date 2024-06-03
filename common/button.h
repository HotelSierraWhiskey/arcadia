#ifndef BUTTON_H
#define BUTTON_H

#include "includes.h"

typedef enum
{
	BUTTON_ID_DEBUG,
	BUTTON_ID_BOOT,
	//
	BUTTON_ID_NUM_BUTTONS
} BUTTON_id_t;

void BUTTON_update_buttons	(void);
bool BUTTON_is_pressed		(const BUTTON_id_t button);

#endif