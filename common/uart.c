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
	uint32_t u32_baud = 36000;

	// Configure pins
	IO_pin_gpio_init(IO_PIN_PA24);	// init TX
	IO_pin_gpio_init(IO_PIN_PA25);	// init RX
	IO_pin_pmux_enable(IO_PIN_PA24, IO_PERIPHERAL_FUNCTION_C);	// Enable peripheral function for pin TX
	IO_pin_pmux_enable(IO_PIN_PA25, IO_PERIPHERAL_FUNCTION_C);	// Enable peripheral function for pin RX

	// Enable the peripheral bus for SERCOM3
	PM_REGS->PM_APBCMASK |= PM_APBCMASK_SERCOM3(1);

	// Use GCLK 0 to clock SERCOM 3
	GCLK_REGS->GCLK_CLKCTRL = 	GCLK_CLKCTRL_GEN(GCLK_CLKCTRL_GEN_GCLK0_Val) | 
								GCLK_CLKCTRL_ID(GCLK_CLKCTRL_ID_SERCOM3_CORE_Val) | 
								GCLK_CLKCTRL_CLKEN(1);


	SERCOM3_REGS->USART_INT.SERCOM_CTRLA = 	SERCOM_USART_INT_CTRLA_MODE(SERCOM_USART_INT_CTRLA_MODE_USART_INT_CLK_Val) |	// Use internal clock
											SERCOM_USART_INT_CTRLA_CMODE(SERCOM_USART_INT_CTRLA_CMODE_ASYNC_Val) |			// Async mode
											SERCOM_USART_INT_CTRLA_DORD(SERCOM_USART_INT_CTRLA_DORD_LSB_Val) |				// Send LSB
											SERCOM_USART_INT_CTRLA_RXPO(SERCOM_USART_INT_CTRLA_RXPO_PAD3_Val) |				// Pad 3 for RX
											SERCOM_USART_INT_CTRLA_TXPO(SERCOM_USART_INT_CTRLA_TXPO_PAD2_Val);				// Pad 2 for TX

	SERCOM3_REGS->USART_INT.SERCOM_CTRLB =  SERCOM_USART_INT_CTRLB_SBMODE(SERCOM_USART_INT_CTRLB_SBMODE_1_BIT_Val) |	// Single stop bit
											SERCOM_USART_INT_CTRLB_RXEN(1) |											// Enable RX
											SERCOM_USART_INT_CTRLB_TXEN(1) |											// Enable TX
											SERCOM_USART_INT_CTRLB_CHSIZE(SERCOM_USART_INT_CTRLB_CHSIZE_8_BIT_Val);		// 8 bit char size

	SERCOM3_REGS->USART_INT.SERCOM_INTENSET = 	SERCOM_USART_INT_INTENCLR_TXC(1) |
												SERCOM_USART_INT_INTENCLR_DRE(1);

	SERCOM3_REGS->USART_INT.SERCOM_BAUD = u32_baud;

	SERCOM3_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_ENABLE(1);
}

void UART_tx_char(const char data)
{
	SERCOM3_REGS->USART_INT.SERCOM_DATA = data;

	while(!(SERCOM3_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_TXC(1)))
	{
		continue;
	}
}