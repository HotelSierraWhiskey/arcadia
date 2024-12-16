#include "utils.h"
#include "chrono.h"

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	Triggers a system crash by dereferencing an invalid pointer
 * 	
 *	@warning This will trigger a Hard Fault interrupt
 *
 ****************************************************************************************************/
void NORETURN UTILS_error_crash(void)
{
	// Let the debug uart buffer flush out
	CHRONO_delay_ms(100);

	volatile uint32_t *p = (volatile uint32_t *)0xDEADBEEF;
	uint32_t value = *p;
	(void)value;

	// Not reached
	while(1);
}

bool UTILS_string_to_u32(const char *kpc_str, uint32_t *u32_out)
{
    char *endptr = NULL;
    
	if (kpc_str == NULL || u32_out == NULL)
    {
        return false;
    }

    uint32_t u32_result = strtoul(kpc_str, &endptr, 0);

    if (endptr == kpc_str || *endptr != '\0' || !isdigit((unsigned char)kpc_str[0]))
    {
        return false;
    }

    if (u32_result > UINT32_MAX)
    {
        return false;
    }

    *u32_out = (uint32_t)u32_result;

    return true;
}
