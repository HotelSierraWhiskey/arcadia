#include "nvmctrl.h"
#include "utils.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define NVMCTRL_LOG_DBG(fmt, ...)   		SHELL_printf("%-10s" fmt, "[NVMCTRL]", ##__VA_ARGS__)

#define NVMCTRL_COMMAND_ERASE_ROW			(0x02U)
#define NVMCTRL_COMMAND_WRITE_PAGE			(0x04U)
#define NVMCTRL_COMMAND_PAGE_BUFFER_CLEAR	(0x44U)
#define NVMCTRL_EXEC_KEY					(0xA5U)

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static void 	NVMCTRL_exec	(uint8_t command);

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void NVMCTRL_init(void)
{
	// Enable AHB clock for NVMCTRL
	MCLK_REGS->MCLK_AHBMASK |= MCLK_AHBMASK_NVMCTRL(1);

	// Enable APBB clock for NVMCTRL
	MCLK_REGS->MCLK_APBBMASK |= MCLK_APBBMASK_NVMCTRL(1);

	// Enable the NVM Ready interrupt
	NVMCTRL_REGS->NVMCTRL_INTENSET |= NVMCTRL_INTENSET_READY(1);
}

void NVMCTRL_write_page(uint32_t u32_addr, uint8_t * pu8_buffer)
{
	uint16_t u16_data;
	uint32_t u32_orig_addr = u32_addr;

	NVMCTRL_exec(NVMCTRL_COMMAND_PAGE_BUFFER_CLEAR);

	u32_addr >>= 1;

	for (uint16_t i = 0; i < NVMCTRL_PAGE_SIZE; i += 2)
	{
		u16_data = pu8_buffer[i];

		if (i < (NVMCTRL_PAGE_SIZE - 1))
		{
			u16_data |= (pu8_buffer[i + 1] << 8);
		}

		NVMCTRL_MEMORY[u32_addr++] = u16_data;
	}

	NVMCTRL_REGS->NVMCTRL_ADDR = u32_addr;

	NVMCTRL_exec(NVMCTRL_COMMAND_WRITE_PAGE);
	NVMCTRL_LOG_DBG("Page write (0x%05X)\r\n", u32_orig_addr);
}

void NVMCTRL_erase_row(uint32_t u32_addr)
{
	NVMCTRL_REGS->NVMCTRL_ADDR = u32_addr >> 1;
	NVMCTRL_exec(NVMCTRL_COMMAND_ERASE_ROW);
	NVMCTRL_LOG_DBG("Row erase (0x%05X - 0x%05X)\r\n", u32_addr, u32_addr + 0x100 - 1);
}

static void NVMCTRL_exec(uint8_t command)
{
	NVMCTRL_REGS->NVMCTRL_CTRLA = (NVMCTRL_EXEC_KEY << 8) | command;

	// Wait for execution to finish
	while ((NVMCTRL_REGS->NVMCTRL_INTFLAG & NVMCTRL_INTFLAG_READY(1)) == 0)
	{
		continue;
	}
}

uint8_t	NVMCTRL_shell_erase(uint8_t argc, char ** argv)
{
	bool 		b_res = false;
	uint32_t 	u32_addr;
	
	if (argc == 1)
	{
		if (UTILS_string_to_u32(argv[0], &u32_addr))
		{
			if (u32_addr % NVMCTRL_PAGE_SIZE == 0)
			{
				NVMCTRL_erase_row(u32_addr);

				b_res = true;
			}
		}
	}
	if (!b_res)
	{
		SHELL_printf("Usage: nvm erase <addr>\r\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

uint8_t NVMCTRL_shell_read(uint8_t argc, char ** argv)
{
	bool 		b_res = false;
	uint32_t 	u32_addr;
	uint8_t 	pu8_buffer[NVMCTRL_PAGE_SIZE];

	if (argc == 1)
	{
		if (UTILS_string_to_u32(argv[0], &u32_addr))
		{
			if (u32_addr % NVMCTRL_PAGE_SIZE == 0)
			{
				for (uint16_t i = 0; i < NVMCTRL_PAGE_SIZE; i++)
				{
					pu8_buffer[i] = ((uint8_t *)NVMCTRL_MEMORY)[u32_addr + i];
				}

				SHELL_SEPARATOR();
				for (uint16_t i = 0; i < NVMCTRL_PAGE_SIZE; i++)
				{
					SHELL_printf("%02X ", pu8_buffer[i]);

					if ((i + 1) % 16 == 0)
					{
						SHELL_printf("\r\n");
					}
				}
				SHELL_SEPARATOR();

				b_res = true;
			}
		}
	}
	
	if (!b_res)
	{
		SHELL_printf("Usage: nvm read <addr>\r\n");
	}

	return SHELL_COMMAND_SUCCESS;
}

uint8_t	NVMCTRL_shell_write(uint8_t argc, char ** argv)
{
	bool 		b_res = false;
	uint32_t 	u32_addr;
	uint32_t 	u32_num_bytes;
	uint32_t 	u32_data;
	uint8_t		pu8_buffer[NVMCTRL_PAGE_SIZE];

	memset(pu8_buffer, 0, NVMCTRL_PAGE_SIZE);

	if (argc >= 3)
	{
		if (UTILS_string_to_u32(argv[0], &u32_addr))
		{
			if (u32_addr % NVMCTRL_PAGE_SIZE == 0)
			{
				b_res = true;
			}
		}

		if (b_res && UTILS_string_to_u32(argv[1], &u32_num_bytes))
		{
			if (u32_num_bytes > NVMCTRL_PAGE_SIZE)
			{
				b_res = false;
			}
		}

		for (uint8_t i = 0; i < u32_num_bytes; i++)
		{
			if (b_res && UTILS_string_to_u32(argv[2 + i], &u32_data))
			{
				pu8_buffer[i] = (uint8_t)u32_data;
			}
			else
			{
				SHELL_printf("Error: %s\r\n", argv[2 + i]);
				b_res = false;
				break;
			}
		}
	}
	
	if (b_res)
	{
		NVMCTRL_write_page(u32_addr, pu8_buffer);
	}
	else
	{
		SHELL_printf("Usage: nvm write <addr> <num_bytes> <...>\r\n");
	}

	return SHELL_COMMAND_SUCCESS;
}