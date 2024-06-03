#include "error.h"

uint8_t ERROR_crash(void)
{
	uint8_t u8_a = 0;
    uint8_t u8_b = 1 / u8_a;
	return u8_b;
}
