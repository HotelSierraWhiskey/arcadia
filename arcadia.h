#ifndef ARCADIA_H
#define ARCADIA_H

typedef enum _ARCADIA_task_id
{
	ARCADIA_TASK_ID_SHELL = 0,
	ARCADIA_TASK_ID_2,
	ARCADIA_TASK_ID_3,
	//////////
	ARCADIA_TASK_ID_NUM_IDS
} ARCADIA_task_id;

void ARCADIA_start(void);

#endif // ARCADIA_H