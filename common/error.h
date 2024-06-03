#ifndef ERROR_H
#define ERROR_H

#include "includes.h"

#define ASSERT(x) do { \
    if (!(x)) { \
        ERROR_crash(); \
    } \
} while (0)

uint8_t ERROR_crash(void) __attribute__((used));

#endif