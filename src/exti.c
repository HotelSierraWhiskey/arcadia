#include "exti.h"
#include "io.h"
#include "utils.h"
#include "shell.h"
#include "arcadia.h"
#include "chrono.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define EXTI_LOG_DBG(fmt, ...)   		SHELL_printf("\r%-12s" fmt, "[EXTI]", ##__VA_ARGS__)
#define EXTI_LOG_WARN(fmt, ...)   		SHELL_PRINT_WARNING("\r%-12s" fmt, "[EXTI]", ##__VA_ARGS__)

#define EXTI_MAX_SOURCES	(16U)

typedef enum _EXTI_edge_detection_id_
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

typedef struct _EXTI_source_config
{
	IO_pin_id_t					pin_id;
	uint8_t						u8_extint;
	EXTI_edge_detection_id_t	edge_detection_id;
	const char * const			kpc_name;
} EXTI_source_config_t;

typedef struct _EXTI_source_state
{
	bool						b_asserted;
	uint32_t					u32_last_event;
} EXTI_source_state_t;

typedef struct _EXTI_source_entry
{
	EXTI_source_config_t		config;
	EXTI_source_state_t			state;
} EXTI_source_entry_t;

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

// None

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

/**
 *	Registry of logical external interrupt channels
 */
static EXTI_source_entry_t EXTI_source_configs[EXTI_SOURCE_ID_NUM_IDS] =
{
	[EXTI_SOURCE_ID_DEBUG_BUTTON] =
	{
		.config =
		{
			.pin_id				= IO_PIN_ID_PA09,
			.u8_extint			= 9,
			.edge_detection_id 	= EXTI_EDGE_DETECTION_ID_BOTH,
			.kpc_name			= "DEBUG_BUTTON"
		}
	}
};

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void EXTI_init(void)
{
	ASSERT(EXTI_SOURCE_ID_NUM_IDS <= EXTI_MAX_SOURCES);

	EXTI_source_config_t * p_source_config;

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

	// Init all registered sources
	for (uint8_t i = 0; i < EXTI_SOURCE_ID_NUM_IDS; i++)
	{
		p_source_config = &EXTI_source_configs[i].config;

		// Configure edge detection. EIC has two configuration registers, each contains 8 source configs
		if (p_source_config->u8_extint < (EXTI_MAX_SOURCES / 2))
		{
			EIC_REGS->EIC_CONFIG[0] |= p_source_config->edge_detection_id << p_source_config->u8_extint;
		}
		else
		{
			EIC_REGS->EIC_CONFIG[1] |= p_source_config->edge_detection_id << (p_source_config->u8_extint / 2);
		}

		// Enable interrupt for source
		EIC_REGS->EIC_INTENSET |= (1 << p_source_config->u8_extint);

		// Enable the peripheral function for the pin
		IO_enable_peripheral_function_for_pin(p_source_config->pin_id, IO_PERIPHERAL_FUNCTION_A);
	}

	// Enable EIC
	EIC_REGS->EIC_CTRLA |= EIC_CTRLA_ENABLE_Msk;

	while (EIC_REGS->EIC_SYNCBUSY & EIC_SYNCBUSY_ENABLE_Msk)
	{
		continue;
	}

	// Enable interrupts for EIC
	NVIC_EnableIRQ(EIC_IRQn);
}

IO_pin_id_t	EXTI_get_pin_from_source(EXTI_source_id_t source)
{
	ASSERT(source < EXTI_SOURCE_ID_NUM_IDS);
	return EXTI_source_configs[source].config.pin_id;
}

bool EXTI_source_asserted(EXTI_source_id_t source)
{
	ASSERT(source < EXTI_SOURCE_ID_NUM_IDS);
	return EXTI_source_configs[source].state.b_asserted;
}

void EXTI_deassert_source(EXTI_source_id_t source)
{
	ASSERT(source < EXTI_SOURCE_ID_NUM_IDS);
	EXTI_source_configs[source].state.b_asserted = false;
}

void EXTI_enable_isr(EXTI_source_id_t source)
{
	ASSERT(source < EXTI_SOURCE_ID_NUM_IDS);
	EIC_REGS->EIC_INTENSET |= (1 << EXTI_source_configs[source].config.u8_extint);
}

void EXTI_disable_isr(EXTI_source_id_t source)
{
	ASSERT(source < EXTI_SOURCE_ID_NUM_IDS);
	EIC_REGS->EIC_INTENCLR |= (1 << EXTI_source_configs[source].config.u8_extint);
}

void EXTI_update(uint32_t u32_sources)
{
	EXTI_source_entry_t * p_entry;
	IO_pin_id_t pin_id;

	for (uint8_t u8_source = 0; u8_source < EXTI_SOURCE_ID_NUM_IDS; u8_source++)
	{
		p_entry = &EXTI_source_configs[u8_source];

		if (u32_sources & (1 << u8_source))
		{
			pin_id = EXTI_get_pin_from_source(u8_source);
			EXTI_LOG_DBG("Source %s on %s %s\n", 
				p_entry->config.kpc_name, IO_get_pin_string(pin_id), IO_read_pin(pin_id) == IO_PIN_STATE_HIGH ? "asserted": "deasserted");
			EXTI_enable_isr(u8_source);
		}
	}
}

void irqEIC(void)
{
	EXTI_source_entry_t * p_entry;
	ARCADIA_msg_t msg =
	{
		.id 				= ARCADIA_MSG_ID_CHRONO_DEBOUNCE_EXTI,
		.b_sent_from_isr 	= true,
	};

	for (uint8_t u8_source = 0; u8_source < EXTI_SOURCE_ID_NUM_IDS; u8_source++)
	{
		p_entry = &EXTI_source_configs[u8_source];

		if (EIC_REGS->EIC_INTFLAG & (1 << p_entry->config.u8_extint))
		{
			// Disable the interrupt, re-enable when debounce timer expires
			EXTI_disable_isr(u8_source);

			// TODO: Right now we only handle detection on both edges
			if (EXTI_EDGE_DETECTION_ID_BOTH == p_entry->config.edge_detection_id)
			{
				p_entry->state.b_asserted = !p_entry->state.b_asserted;
			}

			EIC_REGS->EIC_INTFLAG |= (1 << p_entry->config.u8_extint);

			msg.payload.chrono_payload_debounce_exti.u32_sources |= (1 << u8_source);
			p_entry->state.u32_last_event = CHRONO_get_ticks();
		}
	}

	// Tell CHRONO to set a timer for exti source ISR re-enable
	ARCADIA_send_from_isr(ARCADIA_TASK_ID_CHRONO, &msg);
	
	NVIC_ClearPendingIRQ(EIC_IRQn);
}
