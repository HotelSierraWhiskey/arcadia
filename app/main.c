#include "io.h"
#include "button.h"
#include "clock.h"
#include "systick.h"
#include "sys_time.h"


#define DELAY 1000

bool b_state = false;

void blink(void)
{
    static uint32_t su32_last_update = 0;

    if (SYS_TIME_since(su32_last_update) >= DELAY)
    {
        su32_last_update = SYS_TIME_now();
        b_state = !b_state;
    }

    if (b_state)
    {
        IO_pin_assert(IO_PIN_PA02);
    }
    else
    {
        IO_pin_deassert(IO_PIN_PA02);
    }
}


int main(void)
{

    // safeloop
    for (uint32_t i = 0; i < 400000; i++)
    {
        __asm("nop");
    }

    CLOCK_osc8m_init();
    CLOCK_gclock_init(CLOCK_GCLOCK_ID_0, CLOCK_SOURCE_OSC8M);

    IO_init();

    IO_pin_gpio_init(IO_PIN_PA00);
    IO_pin_gpio_init(IO_PIN_PA01);
    IO_pin_gpio_init(IO_PIN_PA02);
    IO_pin_gpio_init(IO_PIN_PA03);
    IO_pin_gpio_init(IO_PIN_PA04);
    IO_pin_gpio_init(IO_PIN_PA05);
    IO_pin_gpio_init(IO_PIN_PA06);

    SYSTICK_init();
    CLOCK_main_clock_output_enable(IO_PIN_PA27);


    while (true)
    {
        // blink();

        // BUTTON_update_buttons();

        // if (BUTTON_is_pressed(BUTTON_ID_DEBUG))
        // {
        //     IO_pin_assert(IO_PIN_PA02);
        //     IO_pin_assert(IO_PIN_PA03);
        //     IO_pin_assert(IO_PIN_PA04);
        //     IO_pin_assert(IO_PIN_PA05);
        //     CLOCK_main_clock_output_enable(IO_PIN_PA27);
        // }
        // else
        // {
        //     IO_pin_deassert(IO_PIN_PA02);
        //     IO_pin_deassert(IO_PIN_PA03);
        //     IO_pin_deassert(IO_PIN_PA04);
        //     IO_pin_deassert(IO_PIN_PA05);
        //     CLOCK_main_clock_output_disable(IO_PIN_PA27);
        // }
    }
}