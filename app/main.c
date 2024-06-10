#include "io.h"
#include "button.h"
#include "clock.h"
#include "systick.h"
#include "sys_time.h"
#include "uart.h"


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
    SYSTICK_init();

    IO_init();

    IO_pin_gpio_init(IO_PIN_PA00);  // input: boot button
    IO_pin_gpio_init(IO_PIN_PA01);  // input: debug button

    IO_pin_gpio_init(IO_PIN_PA02);  // output: debug led 1
    IO_pin_gpio_init(IO_PIN_PA03);  // output: debug led 2
    IO_pin_gpio_init(IO_PIN_PA04);  // output: debug led 3
    IO_pin_gpio_init(IO_PIN_PA05);  // output: debug led 4

    UART_init();
    IO_pin_assert(IO_PIN_PA02);

    while (true)
    {
        BUTTON_update_buttons();

        if (BUTTON_is_pressed(BUTTON_ID_DEBUG))
        {
            // IO_pin_assert(IO_PIN_PA02);
            IO_pin_assert(IO_PIN_PA03);
            IO_pin_assert(IO_PIN_PA04);
            IO_pin_assert(IO_PIN_PA05);
            // UART_tx_char('U');
        }
        else
        {
            // IO_pin_deassert(IO_PIN_PA02);
            IO_pin_deassert(IO_PIN_PA03);
            IO_pin_deassert(IO_PIN_PA04);
            IO_pin_deassert(IO_PIN_PA05);
        }
    }
}