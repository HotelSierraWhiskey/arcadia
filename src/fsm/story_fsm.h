#ifndef STORY_FSM_H
#define STORY_FSM_H

#include "fsm_event.h"

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void 		STORY_FSM_init				(void);
void 		STORY_FSM_load_arcproject				(const char * kpc_fname);
void 		STORY_FSM_handle_event		(FSM_EVENT_t event);

#endif // STORY_FSM_H
