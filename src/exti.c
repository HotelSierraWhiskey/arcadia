#include "exti.h"
#include "io.h"
#include "shell.h"

void EXTI_init(void)
{
    // Enable APB clock for EIC
    MCLK_REGS->MCLK_APBAMASK |= MCLK_APBAMASK_EIC_Msk;

    // Enable Generic Clock (GCLK0 assumed) for EIC
    GCLK_REGS->GCLK_PCHCTRL[EIC_GCLK_ID] = GCLK_PCHCTRL_CHEN_Msk | GCLK_PCHCTRL_GEN_GCLK0;

    // Wait until GCLK is synchronized
    while (!(GCLK_REGS->GCLK_PCHCTRL[EIC_GCLK_ID] & GCLK_PCHCTRL_CHEN_Msk));

    // Disable EIC before configuring
    EIC_REGS->EIC_CTRLA &= ~EIC_CTRLA_ENABLE_Msk;
    while (EIC_REGS->EIC_SYNCBUSY & EIC_SYNCBUSY_ENABLE_Msk);

    // Configure EXTINT[9] (PA09) for rising-edge detection
    EIC_REGS->EIC_CONFIG[1] &= ~(0x7u << 4); // Clear previous config (SENSE1 is bits [6:4])
    EIC_REGS->EIC_CONFIG[1] |= (0x1u << 4);  // Set rising-edge detection (0x1 = RISE)

    // Optional: disable filtering if not needed
    EIC_REGS->EIC_CONFIG[1] &= ~(1u << 7);   // Clear FILTEN1 (bit 7 for EXTINT[9])

    // Clear any pending interrupts
    EIC_REGS->EIC_INTFLAG = (1 << 9);

    // Enable interrupt for EXTINT[9]
    EIC_REGS->EIC_INTENSET = (1 << 9);

    // Configure PA09 pin for EXTINT[9] (Peripheral A for PA09)
    IO_enable_peripheral_function_for_pin(IO_PIN_ID_PA09, IO_PERIPHERAL_FUNCTION_A);

    // Enable EIC
    EIC_REGS->EIC_CTRLA |= EIC_CTRLA_ENABLE_Msk;
    while (EIC_REGS->EIC_SYNCBUSY & EIC_SYNCBUSY_ENABLE_Msk);

    // Enable interrupt in NVIC
    NVIC_EnableIRQ(EIC_IRQn);
}

void irqEIC(void)
{
    if (EIC_REGS->EIC_INTFLAG & (1 << 9))
    {
        EIC_REGS->EIC_INTFLAG = (1 << 9); // Clear interrupt flag for EXTINT[9]
        SHELL_printf("Wowie!\n");
    }
}
