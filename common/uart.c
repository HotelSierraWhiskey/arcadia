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
	uint32_t u32_baud = (CLOCK_FREQ) / (115200 * 10);

	// Enable the peripheral bus for SERCOM3
	PM_REGS->PM_APBCMASK |= PM_APBCMASK_SERCOM3(1);

	// Use GCLK 0 to clock SERCOM 3
	GCLK_REGS->GCLK_CLKCTRL = 	GCLK_CLKCTRL_GEN(GCLK_CLKCTRL_GEN_GCLK0_Val) | 
								GCLK_CLKCTRL_ID(GCLK_CLKCTRL_ID_SERCOM3_CORE_Val) | 
								GCLK_CLKCTRL_CLKEN(1);

	// Configure pins
	IO_pin_gpio_init(IO_PIN_PA24);
	IO_pin_gpio_init(IO_PIN_PA25);
	IO_pin_pmux_enable(IO_PIN_PA24, IO_PERIPHERAL_FUNCTION_C);
	IO_pin_pmux_enable(IO_PIN_PA24, IO_PERIPHERAL_FUNCTION_C);

	SERCOM3_REGS->USART_INT.SERCOM_CTRLA = 	SERCOM_USART_INT_CTRLA_MODE(SERCOM_USART_INT_CTRLA_MODE_USART_INT_CLK_Val) | 
											SERCOM_USART_INT_CTRLA_CMODE(SERCOM_USART_INT_CTRLA_CMODE_ASYNC_Val) |
											SERCOM_USART_INT_CTRLA_DORD(SERCOM_USART_INT_CTRLA_DORD_LSB) |
											SERCOM_USART_INT_CTRLA_RXPO(SERCOM_USART_INT_CTRLA_RXPO_PAD3) |
											SERCOM_USART_INT_CTRLA_TXPO(SERCOM_USART_INT_CTRLA_TXPO_PAD2);

	SERCOM3_REGS->USART_INT.SERCOM_CTRLB =  SERCOM_USART_INT_CTRLB_SBMODE(SERCOM_USART_INT_CTRLB_SBMODE_1_BIT_Val) |
											SERCOM_USART_INT_CTRLB_RXEN(1) |
											SERCOM_USART_INT_CTRLB_TXEN(1) |
											SERCOM_USART_INT_CTRLB_CHSIZE(SERCOM_USART_INT_CTRLB_CHSIZE_8_BIT_Val);

	SERCOM3_REGS->USART_INT.SERCOM_BAUD = u32_baud;

	SERCOM3_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_ENABLE(1);
}




void UART_tx_char(const char data)
{
	while(!(SERCOM3_REGS->USART_INT.SERCOM_INTFLAG) & SERCOM_USART_INT_INTFLAG_DRE(1))
	{
		continue;
	}

	SERCOM3_REGS->USART_INT.SERCOM_DATA = data;
}