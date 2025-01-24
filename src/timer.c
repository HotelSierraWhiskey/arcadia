#include "timer.h"
#include "io.h"
#include "utils.h"
#include "arcadia.h"
#include "chrono_payload.h"
#include "chrono.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define TIMER_LOG_DBG(fmt, ...)   			SHELL_printf("\r%-12s" fmt, "[TIMER]", ##__VA_ARGS__)

#define TIMER_CTRLA_ENABLE					(TC_CTRLA_MODE_COUNT16 | TC_CTRLA_PRESCALER_DIV1024 | TC_CTRLA_ENABLE(1))
#define TIMER_CTRLA_DISABLE					(TC_CTRLA_ENABLE(0))

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void 		TIMER_on_match			(const TIMER_id_t k_timer_id);
static void 		TIMER_config			(const TIMER_id_t k_timer_id, uint16_t u16_period, TIMER_mode_t mode);

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

/**
 *	The timer pool
 */
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

/**
 *	String representations of timer operation modes
 */
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

/****************************************************************************************************
 *	Allocates and configures a timer from the timer pool
 *
 * 	@param[in] u16_period 	The number of seconds before the timer elapses
 * 	@param[in] mode 		The operation mode of the timer
 * 
 *	@return A timer iD if one was allocated, else `TIMER_INVALID` if the pool was empty
 *
 ****************************************************************************************************/
TIMER_id_t TIMER_alloc(uint16_t u16_period, TIMER_mode_t mode)
{
	TIMER_id_t timer_id = TIMER_INVALID;

	for (uint8_t i = 0; i < TIMER_ID_NUM_TIMERS; i++)
	{
		if (TIMER_AVAILABLE == p_timer_pool[i].u16_period)
		{
			TIMER_config(i, u16_period, mode);
			timer_id = i;
			break;
		}
	}

	return timer_id;
}

/****************************************************************************************************
 *	Retrieves a timer's logical channel information
 *
 * 	@param[in] k_timer_id 	The ID of the timer to retrieve
 * 
 *	@return the associated entry in the `p_timer_pool`
 *
 ****************************************************************************************************/
const TIMER_info_t * TIMER_get_timer_info(const TIMER_id_t k_timer_id)
{
	ASSERT(k_timer_id < TIMER_ID_NUM_TIMERS);
	return &p_timer_pool[k_timer_id];
}

/****************************************************************************************************
 *	Starts a timer
 *
 * 	Enables the timer's IRQ line, and engages the associated timer peripheral.
 *
 * 	@param[in] k_timer_id 	The ID of the timer to start
 *
 ****************************************************************************************************/
void TIMER_start(const TIMER_id_t k_timer_id)
{
	ASSERT(k_timer_id < TIMER_ID_NUM_TIMERS);

	TIMER_info_t * p_timer = &p_timer_pool[k_timer_id];

	NVIC_EnableIRQ(p_timer->u8_irq_id);

	p_timer->p_timer_regs->COUNT16.TC_CTRLA |= TIMER_CTRLA_ENABLE;

	while (p_timer->p_timer_regs->COUNT16.TC_SYNCBUSY & TC_SYNCBUSY_ENABLE(1))
	{
		continue;
	}
}

/****************************************************************************************************
 *	Stops a timer
 *
 * 	Disables a hardware timer completely, clears its match flag and disables its IRQ line.
 * 	In addition, the timer's previously configured `u16_period` member will be reset to 0.
 *
 * 	@param[in] k_timer_id 	The ID of the timer to stop
 *
 ****************************************************************************************************/
void TIMER_stop(const TIMER_id_t k_timer_id)
{
	ASSERT(k_timer_id < TIMER_ID_NUM_TIMERS);

	TIMER_info_t * p_timer = &p_timer_pool[k_timer_id];

	p_timer->p_timer_regs->COUNT16.TC_CTRLA &= ~TIMER_CTRLA_ENABLE;
	p_timer->p_timer_regs->COUNT16.TC_INTFLAG = TC_INTFLAG_MC0(1);
	p_timer->u16_period = TIMER_AVAILABLE;

	NVIC_DisableIRQ(p_timer->u8_irq_id);
}

/****************************************************************************************************
 *	Retrieves the raw value in the timer's COUNT register
 *
 * 	This value needs to be manaully read-synchronized via a `TC_CTRLBSET_CMD_READSYNC` command,
 * 	And should therefore be used sparingly/ for debugging/ development only.
 *
 * 	@param[in] k_timer_id 	The ID of the timer to read
 *
 * 	@return The value in the timer's COUNT register
 ****************************************************************************************************/
uint16_t TIMER_get_timer_count(const TIMER_id_t k_timer_id)
{
	ASSERT(k_timer_id < TIMER_ID_NUM_TIMERS);

	TIMER_info_t * 	p_timer = &p_timer_pool[k_timer_id];

	p_timer->p_timer_regs->COUNT16.TC_CTRLBSET = TC_CTRLBSET_CMD_READSYNC;

	// fixme?
	CHRONO_delay_ms(10);

	while (p_timer->p_timer_regs->COUNT16.TC_SYNCBUSY & TC_SYNCBUSY_COUNT(1))
	{
		continue;
	}

	return p_timer->p_timer_regs->COUNT16.TC_COUNT;
}

/****************************************************************************************************
 *	Configures a hardware timer
 *
 * 	@param[in] k_timer_id 	The ID of the timer to configure
 * 	@param[in] u16_period 	The number of seconds before the timer's associated ISR fires
 * 	@param[in] mode 		The desired operation mode
 *
 ****************************************************************************************************/
static void TIMER_config(const TIMER_id_t k_timer_id, uint16_t u16_period, TIMER_mode_t mode)
{
	TIMER_info_t * p_timer = &p_timer_pool[k_timer_id];

	p_timer->u16_period = u16_period;
	p_timer->mode = mode;

	p_timer->p_timer_regs->COUNT16.TC_CC[0] = (u16_period * TIMER_PRESCALED_SECOND_COUNT_VALUE);
}

/****************************************************************************************************
 *	Generic handler used in each hardware timer's ISR.
 *
 * 	A timer elapses when its MC0 (Match/ Compare) flag is set.
 * 	A timer's MC0 flag is set when its COUNT register matches the CC value for which the timer was 
 * 	configured.
 * 
 * 	When a timer elapses, an `ARCADIA_MSG_ID_CHRONO_TIMER_ELAPSED` message that includes the timer's 
 * 	ID is sent from ISR context to the CHRONO task. The ID of the elapsed timer is used in task context
 * 	for message scheduling.
 * 	
 * 	@param[in] k_timer_id The ID of the timer that just elapsed
 * 	
 ****************************************************************************************************/
static void TIMER_on_match(const TIMER_id_t k_timer_id)
{
	TIMER_info_t * 					p_timer = &p_timer_pool[k_timer_id];
	ARCADIA_msg_t 					msg;
	CHRONO_PAYLOAD_timer_elapsed_t 	payload = {.timer_id = k_timer_id};
	
	msg.id = ARCADIA_MSG_ID_CHRONO_TIMER_ELAPSED;
	msg.payload.chrono_payload_timer_elapsed = payload;
	msg.from = ARCADIA_TASK_ID_CHRONO;

	if ((p_timer->p_timer_regs->COUNT16.TC_INTFLAG & TC_INTFLAG_MC0(1)) != 0)
	{
		if (p_timer->mode == TIMER_MODE_REPEAT)
		{
			p_timer->p_timer_regs->COUNT16.TC_CTRLBSET = TC_CTRLBSET_CMD_RETRIGGER;
		}
		if (p_timer->mode == TIMER_MODE_SINGLE_SHOT)
		{
			TIMER_stop(k_timer_id);
		}

		ARCADIA_send_from_isr(ARCADIA_TASK_ID_CHRONO, &msg);

		p_timer->p_timer_regs->COUNT16.TC_INTFLAG = TC_INTFLAG_MC0(1);
	}

	NVIC_ClearPendingIRQ(p_timer->u8_irq_id);
}

/****************************************************************************************************
 *	TC0 Interrupt Service Routine
 *
 ****************************************************************************************************/
void irqTC0(void)
{
	TIMER_on_match(TIMER_ID_0);
}

/****************************************************************************************************
 *	TC1 Interrupt Service Routine
 *
 ****************************************************************************************************/
void irqTC1(void)
{
	TIMER_on_match(TIMER_ID_1);
}

/****************************************************************************************************
 *	TC2 Interrupt Service Routine
 *
 ****************************************************************************************************/
void irqTC2(void)
{
	TIMER_on_match(TIMER_ID_2);
}

/****************************************************************************************************
 *	TC3 Interrupt Service Routine
 *
 ****************************************************************************************************/
void irqTC3(void)
{
	TIMER_on_match(TIMER_ID_3);
}

/****************************************************************************************************
 *	S H E L L   F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	Shell utility
 *
 * 	Configures and starts a timer
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
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

/****************************************************************************************************
 *	Shell utility
 *
 * 	Stops a timer
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
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

/****************************************************************************************************
 *	Shell utility
 *
 * 	Displays the status of each timer
 * 
 *	@param[in] argc
 *	@param[in] argv
 *
 *	@return `SHELL_COMMAND_SUCCESS`
 ****************************************************************************************************/
uint8_t	TIMER_shell_info(uint8_t argc, char ** argv)
{
	uint32_t 	u32_current_count;
	uint32_t 	u32_time_remaining;
	uint32_t 	u32_hours;
	uint32_t 	u32_minutes;
	uint32_t 	u32_seconds;
	char 		pc_time_buffer[16] = { 0 };

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
				u32_current_count = TIMER_get_timer_count(i) / TIMER_PRESCALED_SECOND_COUNT_VALUE;

				u32_time_remaining = p_timer_pool[i].u16_period - u32_current_count;

				u32_hours = u32_time_remaining / 3600;
				u32_minutes = (u32_time_remaining % 3600) / 60;
				u32_seconds = u32_time_remaining % 60;

				snprintf(pc_time_buffer, sizeof(pc_time_buffer), "%02lu:%02lu:%02lu", u32_hours, u32_minutes, u32_seconds);
				SHELL_printf("\t%-20s: Running\r\n", "Status");
				SHELL_printf("\t%-20s: %s\r\n", "Mode", kpc_mode_descriptors[p_timer_pool[i].mode]);
				SHELL_printf("\t%-20s: %s\r\n", "Time Remaining", pc_time_buffer);
				memset(pc_time_buffer, 0, sizeof(pc_time_buffer));
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
