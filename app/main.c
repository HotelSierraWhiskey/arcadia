#include "io.h"
#include "button.h"
#include "clock.h"


#define DELAY 100000

int main(void)
{
    CLOCK_osc8m_init();
    CLOCK_gclock_init(CLOCK_GCLOCK_ID_0, CLOCK_SOURCE_OSC8M);

    IO_init();

    IO_pin_gpio_init(IO_PIN_PA00);
    IO_pin_gpio_init(IO_PIN_PA01);
    IO_pin_gpio_init(IO_PIN_PA02);
    IO_pin_gpio_init(IO_PIN_PA03);
    IO_pin_gpio_init(IO_PIN_PA04);
    IO_pin_gpio_init(IO_PIN_PA05);

    while (true)
    {
        BUTTON_update_buttons();

        if (BUTTON_is_pressed(BUTTON_ID_DEBUG))
        {
            IO_pin_assert(IO_PIN_PA02);
            IO_pin_assert(IO_PIN_PA03);
            IO_pin_assert(IO_PIN_PA04);
            IO_pin_assert(IO_PIN_PA05);
        }
        else
        {
            IO_pin_deassert(IO_PIN_PA02);
            IO_pin_deassert(IO_PIN_PA03);
            IO_pin_deassert(IO_PIN_PA04);
            IO_pin_deassert(IO_PIN_PA05);
        }
    }
}