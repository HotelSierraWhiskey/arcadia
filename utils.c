#include "utils.h"

/****************************************************************************************************
 *	Triggers a system crash by dereferencing an invalid pointer
 * 	
 *	@warning This will invoke a Hard Fault interrupt
 *
 ****************************************************************************************************/
void NORETURN UTILS_error_crash(void)
{
	volatile uint32_t *p = (volatile uint32_t *)0xDEADBEEF;
	uint32_t value = *p;
	(void)value;

	// Not reached
	while(1);
}
