#include "timer.h"
#include "io.h"
#include "utils.h"
#include "arcadia.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define TIMER_LOG_DBG(fmt, ...)   			SHELL_printf("%-10s" fmt, "[TIMER]", ##__VA_ARGS__)

#define TIMER_PRESCALED_SECOND_COUNT_VALUE	(32U)
#define TIMER_CTRLA_ENABLE					(TC_CTRLA_MODE_COUNT16 | TC_CTRLA_PRESCALER_DIV1024 | TC_CTRLA_ENABLE(1))
#define TIMER_CTRLA_DISABLE					(TC_CTRLA_ENABLE(0))

typedef struct _TIMER_info
{
	uint16_t			u16_period;
	TIMER_mode_t		mode;
	tc_registers_t *	p_timer_regs;
	uint8_t				u8_irq_id;
} TIMER_info_t;

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static uint16_t 	TIMER_get_time_remaining	(const TIMER_id_t k_timer_id);
static void 		TIMER_on_match				(const TIMER_id_t k_timer_id);

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

static TIMER_info_t p_timer_pool[TIMER_ID_NUM_TIMERS] =
{
	[TIMER_ID_0] =
	{
		.u16_period		= TIMER_AVAILABLE,
		.mode			= TIMER_MODE_SINGLE_SHOT,
		.p_timer_regs 	= TC0_REGS,
		.u8_irq_id		= TC0_IRQn, 
	},
	[TIMER_ID_1] =
	{
		.u16_period		= TIMER_AVAILABLE,
		.mode			= TIMER_MODE_SINGLE_SHOT,
		.p_timer_regs 	= TC1_REGS,
		.u8_irq_id		= TC1_IRQn, 
	},
	[TIMER_ID_2] =
	{
		.u16_period		= TIMER_AVAILABLE,
		.mode			= TIMER_MODE_SINGLE_SHOT,
		.p_timer_regs 	= TC2_REGS,
		.u8_irq_id		= TC2_IRQn, 
	},
	[TIMER_ID_3] =
	{
		.u16_period		= TIMER_AVAILABLE,
		.mode			= TIMER_MODE_SINGLE_SHOT,
		.p_timer_regs 	= TC3_REGS,
		.u8_irq_id		= TC3_IRQn, 
	}
};

const char * const kpc_mode_descriptors[TIMER_MODE_NUM_MODES] =
{
	[TIMER_MODE_SINGLE_SHOT]	= "SINGLE_SHOT",
	[TIMER_MODE_REPEAT]			= "REPEAT"
};

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	Initializes TC0, TC1, TC2, and TC3 in 16-bit mode.
 *
 *	TC0 & TC1 are muxed into peripheral channel 30, TC2 & TC3 are muxed into peripheral channel 31.
 *	All timers have their APB clocks enabled and their "Match or Compare Channel 0" interrupt enabled.
 *	Compare/ Capture registers for each timer are hardcoded to 32.
 * 
 ****************************************************************************************************/
void TIMER_init(void)
{
	IO_config_pin_direction(IO_PIN_ID_PA27, IO_DIRECTION_OUTPUT);

	GCLK_REGS->GCLK_PCHCTRL[30] = 	GCLK_PCHCTRL_CHEN(1) |
									GCLK_PCHCTRL_GEN_GCLK1;

	GCLK_REGS->GCLK_PCHCTRL[31] = 	GCLK_PCHCTRL_CHEN(1) |
									GCLK_PCHCTRL_GEN_GCLK1;

	MCLK_REGS->MCLK_APBCMASK |=	MCLK_APBCMASK_TC0(1) |
								MCLK_APBCMASK_TC1(1) |
								MCLK_APBCMASK_TC2(1) |
								MCLK_APBCMASK_TC3(1);

	for (uint8_t i = 0; i < TIMER_ID_NUM_TIMERS; i++)
	{
		p_timer_pool[i].p_timer_regs->COUNT16.TC_INTENSET = TC_INTENSET_MC0(1);
		NVIC_DisableIRQ(p_timer_pool[i].u8_irq_id);
	}
}

void TIMER_config(const TIMER_id_t k_timer_id, uint16_t u16_period, TIMER_mode_t mode)
{
	TIMER_info_t * p_timer = &p_timer_pool[k_timer_id];

	p_timer->u16_period = u16_period;
	p_timer->mode = mode;

	SHELL_printf("TASK TO NOTIFY %u\r\n", ARCADIA_get_current_task_id());
	SHELL_printf("TASK TO NOTIFY %u\r\n", ARCADIA_get_current_task_id());

	p_timer->p_timer_regs->COUNT16.TC_CC[0] = (u16_period * TIMER_PRESCALED_SECOND_COUNT_VALUE);
}

void TIMER_start(const TIMER_id_t k_timer_id)
{
	TIMER_info_t * p_timer = &p_timer_pool[k_timer_id];

	NVIC_EnableIRQ(p_timer->u8_irq_id);

	p_timer->p_timer_regs->COUNT16.TC_CTRLA |= TIMER_CTRLA_ENABLE;

	while (p_timer->p_timer_regs->COUNT16.TC_SYNCBUSY & TC_SYNCBUSY_ENABLE(1))
	{
		continue;
	}

}

void TIMER_stop(const TIMER_id_t k_timer_id)
{
	TIMER_info_t * p_timer = &p_timer_pool[k_timer_id];

	p_timer->p_timer_regs->COUNT16.TC_CTRLA &= ~TIMER_CTRLA_ENABLE;
	p_timer->p_timer_regs->COUNT16.TC_INTFLAG = TC_INTFLAG_MC0(1);
	p_timer->u16_period = TIMER_AVAILABLE;

	NVIC_DisableIRQ(p_timer->u8_irq_id);
}


static uint16_t TIMER_get_time_remaining(const TIMER_id_t k_timer_id)
{
	TIMER_info_t * 	p_timer = &p_timer_pool[k_timer_id];
	uint16_t		u16_count_val;
	uint16_t		u16_time_remaining = 0;

	p_timer->p_timer_regs->COUNT16.TC_CTRLBSET = TC_CTRLBSET_CMD_READSYNC;

	while (p_timer->p_timer_regs->COUNT16.TC_SYNCBUSY & TC_SYNCBUSY_COUNT(1))
	{
		continue;
	}

	u16_count_val = p_timer->p_timer_regs->COUNT16.TC_COUNT;

	if (p_timer->u16_period > 0)
	{
		u16_time_remaining = u16_count_val / p_timer->u16_period;
	}

	return u16_time_remaining;
}

volatile bool s = true;

static void TIMER_on_match(const TIMER_id_t k_timer_id)
{
	TIMER_info_t * 	p_timer = &p_timer_pool[k_timer_id];
	ARCADIA_msg_t 	msg;

	if ((p_timer->p_timer_regs->COUNT16.TC_INTFLAG & TC_INTFLAG_MC0(1)) != 0)
	{
		if (s)
		{
			IO_set_pin(IO_PIN_ID_PA27, IO_PIN_STATE_HIGH);
			s = false;
		}
		else
		{
			IO_set_pin(IO_PIN_ID_PA27, IO_PIN_STATE_LOW);
			s = true;
		}


		if (p_timer->mode == TIMER_MODE_REPEAT)
		{
			p_timer->p_timer_regs->COUNT16.TC_CTRLBSET = TC_CTRLBSET_CMD_RETRIGGER;
		}
		if (p_timer->mode == TIMER_MODE_SINGLE_SHOT)
		{
			TIMER_stop(k_timer_id);
		}

		msg.from = ARCADIA_TASK_ID_SHELL; //p_timer->task_to_notify;
		msg.id = ARCADIA_MSG_ID_NOOP;
		ARCADIA_send_from_isr(ARCADIA_TASK_ID_SHELL, &msg);

		p_timer->p_timer_regs->COUNT16.TC_INTFLAG = TC_INTFLAG_MC0(1);
	}

	NVIC_ClearPendingIRQ(p_timer->u8_irq_id);
}

void irqTC0(void)
{
	TIMER_on_match(TIMER_ID_0);
}

void irqTC1(void)
{
	TIMER_on_match(TIMER_ID_1);
}

void irqTC2(void)
{
	TIMER_on_match(TIMER_ID_2);
}

void irqTC3(void)
{
	TIMER_on_match(TIMER_ID_3);
}

uint8_t	TIMER_shell_start_timer(uint8_t argc, char ** argv)
{
	uint32_t 		timer_id;
	uint32_t		u32_seconds;
	uint32_t		mode;
	bool			b_res = false;

	if (argc == 3)
	{
		if (UTILS_string_to_u32(argv[0], &timer_id))
		{
			if (timer_id < TIMER_ID_NUM_TIMERS)
			{
				b_res = true;
			}
		}
		if (b_res && UTILS_string_to_u32(argv[1], &u32_seconds))
		{
			if (u32_seconds < 0 || u32_seconds > UINT16_MAX)
			{
				b_res = false;
			}
		}
		if (b_res && UTILS_string_to_u32(argv[2], &mode))
		{
			if (mode > TIMER_MODE_NUM_MODES)
			{
				b_res = false;
			}
		}
	}

	if (b_res)
	{
		if (TIMER_AVAILABLE == p_timer_pool[timer_id].u16_period)
		{
			TIMER_config(timer_id, u32_seconds, mode);
			TIMER_start(timer_id);
			TIMER_LOG_DBG("Timer %u started for %u seconds in %s mode\r\n", 
				timer_id, u32_seconds, kpc_mode_descriptors[mode]);
		}
		else
		{
			TIMER_LOG_DBG("Timer %u is unavailable\r\n", timer_id);
		}
	}
	else
	{
		SHELL_printf("Usage: timer start <id> <seconds> <mode>\r\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

uint8_t	TIMER_shell_stop_timer(uint8_t argc, char ** argv)
{
	uint32_t 		timer_id;
	bool 			b_res = false;
	
	if (argc == 1)
	{
		if (UTILS_string_to_u32(argv[0], &timer_id))
		{
			if (timer_id < TIMER_ID_NUM_TIMERS)
			{
				b_res = true;
			}
		}
	}

	if (b_res)
	{
		TIMER_stop(timer_id);
		TIMER_LOG_DBG("Timer %u stopped\r\n", timer_id);
	}
	else
	{
		SHELL_printf("Usage: timer stop <id>\r\n");
	}

	return SHELL_COMMAND_SUCCESS;

}

uint8_t	TIMER_shell_info(uint8_t argc, char ** argv)
{
	if (argc == 0)
	{
		SHELL_SEPARATOR();
		for (uint8_t i = 0; i < TIMER_ID_NUM_TIMERS; i++)
		{
			SHELL_printf("Timer ID %u:\r\n", i);

			if (TIMER_AVAILABLE == p_timer_pool[i].u16_period)
			{
				SHELL_printf("\t%-20s: Not Running\r\n", "Status");
			}
			else
			{
				SHELL_printf("\t%-20s: Running\r\n", "Status");
				SHELL_printf("\t%-20s: %s\r\n", "Mode", kpc_mode_descriptors[p_timer_pool[i].mode]);
				SHELL_printf("\t%-20s: %u/%u\r\n", "Time Remaining", TIMER_get_time_remaining(i), p_timer_pool[i].u16_period);
			}
			if (i < (TIMER_ID_NUM_TIMERS - 1))
			{
				SHELL_printf("\r\n");
			}
		}
		SHELL_SEPARATOR();
	}
	else
	{
		SHELL_printf("Usage: timer info\r\n");
	}
	return SHELL_COMMAND_SUCCESS;
}
