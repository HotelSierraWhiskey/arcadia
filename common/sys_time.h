#ifndef SYS_TIME_H
#define SYS_TIME_H

#include "systick.h"

#define now() 				SYS_TIME_now()
#define time_since(time)	SYS_TIME_since(time)

static inline uint32_t SYS_TIME_now(void)
{
	return SYSTICK_get_ticks();
}

static inline uint32_t SYS_TIME_since(const uint32_t ku32_time)
{
	return now() - ku32_time;
}

#endif