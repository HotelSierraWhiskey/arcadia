#include "memory_map.h"
#include "includes.h"


static void __attribute__((naked)) start_app(uint32_t pc, uint32_t sp) 
{
    __asm__(
        "ldr r3, =#0xe000ed08\n"
        "str r1, [r3]\n"
        "msr  msp, r1\n"
        "bx r0\n"
    );
}

int main(void)
{
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Warray-bounds"

    uint32_t *app_code  = &_approm_start;
    uint32_t app_sp     = app_code[0];
    uint32_t app_start  = app_code[1];

    #pragma GCC diagnostic pop

    start_app(app_start, app_sp);

    // not reached
    while(true)
    {
        continue;
    }
}