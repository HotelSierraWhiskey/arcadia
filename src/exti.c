#include "exti.h"
#include "io.h"
#include "shell.h"

typedef enum _EXTI_edge_detectionid_
{
	EXTI_EDGE_DETECTION_ID_NONE = 0,
	EXTI_EDGE_DETECTION_ID_RISE,
	EXTI_EDGE_DETECTION_ID_FALL,
	EXTI_EDGE_DETECTION_ID_BOTH,
	EXTI_EDGE_DETECTION_ID_HIGH,
	EXTI_EDGE_DETECTION_ID_LOW,
	//////////
	EXTI_EDGE_DETECTION_NUM_IDS,
} EXTI_edge_detection_id_t;

typedef struct _EXTI_source_config_entry
{
	IO_pin_id_t					pin_id;
	EXTI_edge_detection_id_t	edge_detection_id;
} EXTI_source_config_entry_t;

static EXTI_source_config_entry_t EXTI_source_configs[] =
{
	
};

void EXTI_init(void)
{
    // Enable APB clock for EIC
    MCLK_REGS->MCLK_APBAMASK |= MCLK_APBAMASK_EIC_Msk;

    // Enable GCLK0 for EIC
    GCLK_REGS->GCLK_PCHCTRL[EIC_GCLK_ID] = 	GCLK_PCHCTRL_CHEN(1) |
											GCLK_PCHCTRL_GEN_GCLK0;

    // Wait until GCLK is synchronized
    while ((GCLK_REGS->GCLK_PCHCTRL[EIC_GCLK_ID] & GCLK_PCHCTRL_CHEN(1)) == 0)
	{
		continue;
	}

    // Configure EXTINT[9] (PA09) for rising-edge detection
    EIC_REGS->EIC_CONFIG[1] &= ~(0x7u << 4); // Clear previous config (SENSE1 is bits [6:4])
    EIC_REGS->EIC_CONFIG[1] |= (0x1u << 4);  // Set rising-edge detection (0x1 = RISE)

    // Enable interrupt for EXTINT[9]
    EIC_REGS->EIC_INTENSET = (1 << 9);

    // Configure PA09 pin for EXTINT[9] (Peripheral A for PA09)
    IO_enable_peripheral_function_for_pin(IO_PIN_ID_PA09, IO_PERIPHERAL_FUNCTION_A);

    // Enable EIC
    EIC_REGS->EIC_CTRLA |= EIC_CTRLA_ENABLE_Msk;

    while (EIC_REGS->EIC_SYNCBUSY & EIC_SYNCBUSY_ENABLE_Msk)
	{
		continue;
	}

    // Enable interrupts for EIC
    NVIC_EnableIRQ(EIC_IRQn);
}

void irqEIC(void)
{
    if (EIC_REGS->EIC_INTFLAG & (1 << 9))
    {
        SHELL_printf("Wowie!\n");
        EIC_REGS->EIC_INTFLAG = (1 << 9); // Clear interrupt flag for EXTINT[9]
    }
}
