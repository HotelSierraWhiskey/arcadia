#include "pv035hv_cina5007.h"
#include "utils.h"
#include "io.h"
#include "spi.h"
#include "chrono.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define PV035HV_CINA5007_NRESET_PIN IO_PIN_ID_PA05
#define PV035HV_CINA5007_RS_PIN IO_PIN_ID_PA04

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void PV035HV_CINA5007_init(PV035HV_CINA5007_mode_t mode)
{
	ASSERT(mode < PV035HV_CINA5007_MODE_NUM_MODES);

	switch (mode)
	{
		case PV035HV_CINA5007_MODE_SPI:
		{
			IO_config_pin_direction(PV035HV_CINA5007_NRESET_PIN, IO_DIRECTION_OUTPUT);
			IO_config_pin_direction(PV035HV_CINA5007_RS_PIN, IO_DIRECTION_OUTPUT);

			// Assert NRESET and stabilize
			IO_set_pin(PV035HV_CINA5007_NRESET_PIN, IO_PIN_STATE_LOW);
			CHRONO_delay_ms(120);

			// Deassert NRESET and stabilize
			IO_set_pin(PV035HV_CINA5007_NRESET_PIN, IO_PIN_STATE_HIGH);
			CHRONO_delay_ms(120);

			SPI_init(SPI_CHANNEL_DISPLAY);

			PV035HV_CINA5007_write_command(0x01); // Software Reset
			CHRONO_delay_ms(120);

			PV035HV_CINA5007_write_command(0x11); // Sleep Out
			CHRONO_delay_ms(120);

			PV035HV_CINA5007_write_command(0x3A); // Set Pixel Format
			PV035HV_CINA5007_write_data(0x55);   // 16-bit color (RGB565)

			PV035HV_CINA5007_write_command(0x36); // Memory Access Control
			PV035HV_CINA5007_write_data(0x28);   // Adjust for default orientation

			PV035HV_CINA5007_write_command(0x29); // Display On
		}
	}
}

void PV035HV_CINA5007_write_command(uint8_t u8_cmd)
{
    IO_set_pin(PV035HV_CINA5007_RS_PIN, IO_PIN_STATE_LOW); // Set D/C low for command
    SPI_transfer(SPI_CHANNEL_DISPLAY, u8_cmd);
}

void PV035HV_CINA5007_write_data(uint8_t u8_data)
{
    IO_set_pin(PV035HV_CINA5007_RS_PIN, IO_PIN_STATE_HIGH); // Set D/C high for data
    SPI_transfer(SPI_CHANNEL_DISPLAY, u8_data);
}
