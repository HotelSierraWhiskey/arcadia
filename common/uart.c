#include "uart.h"
#include "io.h"
#include "clock.h"

// debug UART
// peripheral function C
// PA24 - SERCOM3 PAD 2 (tx)
// PA25 - SERCOM3 PAD 3 (rx)

// baud info on p. 413

void UART_init(void)
{
	// Configure pins
	IO_pin_gpio_init(IO_PIN_PA24);	// init TX
	IO_pin_gpio_init(IO_PIN_PA25);	// init RX
	IO_pin_pmux_enable(IO_PIN_PA24, IO_PERIPHERAL_FUNCTION_C);	// Enable peripheral function for pin TX
	IO_pin_pmux_enable(IO_PIN_PA25, IO_PERIPHERAL_FUNCTION_C);	// Enable peripheral function for pin RX

	// IO_pin_gpio_init(IO_PIN_PA06);	// init TX
	// IO_pin_gpio_init(IO_PIN_PA07);	// init RX
	// IO_pin_pmux_enable(IO_PIN_PA06, IO_PERIPHERAL_FUNCTION_D);	// Enable peripheral function for pin TX
	// IO_pin_pmux_enable(IO_PIN_PA07, IO_PERIPHERAL_FUNCTION_D);	// Enable peripheral function for pin RX

	PM_REGS->PM_APBCMASK |= PM_APBCMASK_SERCOM3(1);

	SERCOM3_REGS->USART_INT.SERCOM_CTRLA = SERCOM_USART_INT_CTRLA_SWRST(1);

	while (SERCOM3_REGS->USART_INT.SERCOM_CTRLA & SERCOM_USART_INT_CTRLA_SWRST(1) || 
		SERCOM3_REGS->USART_INT.SERCOM_SYNCBUSY & SERCOM_USART_INT_SYNCBUSY_SWRST(1))
	{
		continue;
	}

	// NVIC_EnableIRQ(SERCOM3_IRQn);

	GCLK_REGS->GCLK_CLKCTRL = 	GCLK_CLKCTRL_GEN(GCLK_CLKCTRL_GEN_GCLK0_Val) | 
								GCLK_CLKCTRL_ID(GCLK_CLKCTRL_ID_SERCOM3_CORE_Val) | 
								GCLK_CLKCTRL_CLKEN(1);

	// Wait for register synchronization
	while (GCLK_REGS->GCLK_STATUS & GCLK_STATUS_SYNCBUSY(1))
	{
		continue;
	}

	SERCOM3_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_MODE(SERCOM_USART_INT_CTRLA_MODE_USART_INT_CLK_Val) |	// Use internal clock
											SERCOM_USART_INT_CTRLA_CMODE(SERCOM_USART_INT_CTRLA_CMODE_ASYNC_Val) |			// Async mode
											SERCOM_USART_INT_CTRLA_DORD(SERCOM_USART_INT_CTRLA_DORD_LSB_Val) |				// Send LSB
											SERCOM_USART_INT_CTRLA_RXPO(SERCOM_USART_INT_CTRLA_RXPO_PAD3_Val) |				// Pad 3 for RX
											SERCOM_USART_INT_CTRLA_TXPO(SERCOM_USART_INT_CTRLA_TXPO_PAD2_Val) |				// Pad 2 for TX
											SERCOM_USART_EXT_CTRLA_SAMPR(SERCOM_USART_INT_CTRLA_SAMPR_16X_ARITHMETIC_Val);	// 16x over-sampling using arithmetic baud rate generation

	SERCOM3_REGS->USART_INT.SERCOM_CTRLB |= SERCOM_USART_INT_CTRLB_SBMODE(SERCOM_USART_INT_CTRLB_SBMODE_1_BIT_Val) |	// Single stop bit
											SERCOM_USART_INT_CTRLB_RXEN(1) |											// Enable RX
											SERCOM_USART_INT_CTRLB_TXEN(1) |											// Enable TX
											SERCOM_USART_INT_CTRLB_CHSIZE(SERCOM_USART_INT_CTRLB_CHSIZE_8_BIT_Val);		// 8 bit char size

	SERCOM3_REGS->USART_INT.SERCOM_INTENSET = 	SERCOM_USART_INT_INTENSET_RXC(1) | 	// RX complete
												SERCOM_USART_INT_INTENSET_TXC(1) | 	// TX complete
												SERCOM_USART_INT_INTENSET_ERROR(1);	// Errors

	SERCOM3_REGS->USART_INT.SERCOM_BAUD = 65535.0f * ( 1.0f - (16.0f) * (float)(115200) / (float)(CLOCK_FREQ));

	SERCOM3_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_ENABLE(1);

	while (SERCOM3_REGS->USART_INT.SERCOM_SYNCBUSY & SERCOM_USART_INT_SYNCBUSY_ENABLE(1))
	{
		continue;
	}
}

void UART_tx_char(const char data)
{
	SERCOM3_REGS->USART_INT.SERCOM_DATA = data;

	while(!(SERCOM3_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_TXC(1)))
	{
		continue;
	}
}

void irqSERCOM3(void)
{
    // IO_pin_assert(IO_PIN_PA04);
}