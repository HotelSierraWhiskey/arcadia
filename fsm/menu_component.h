#ifndef MENU_COMPONENT_H
#define MENU_COMPONENT_H

#include "common.h"

typedef enum _MENU_COMPONENT_type
{
	MENU_COMPONENT_TYPE_STORY = 0,
	//////////
	MENU_COMPONENT_TYPE_NUM_COMPONENTS
} MENU_COMPONENT_type_t;

typedef struct _MENU_COMPONENT_story
{
	char	pc_fname[COMMON_MAX_FNAME_SIZE];
} MENU_COMPONENT_story_t;


typedef struct _MENU_COMPONENT
{
	MENU_COMPONENT_type_t type;
	union
	{
		MENU_COMPONENT_story_t story;
	} component;
} MENU_COMPONENT_t;

#endif // MENU_COMPONENT_H
