#ifndef COMMON_H
#define COMMON_H

#define COMMON_MAX_FNAME_SIZE	(128U)

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <math.h>

#ifndef UNIT_TEST
#ifndef BOOTLOADER
// Do not reorder. FreeRTOS must be included before task.h
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>
#endif // BOOTLOADER

#ifdef MCU_SAMC21N18A
	#include "samc21n18a.h"
#else
	#include "samc21e18a.h"
#endif // MCU_SAMC21N18A

#include "core_cm0plus.h"

#endif // !UNIT_TEST

#endif // COMMON_H
