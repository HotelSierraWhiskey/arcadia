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

/* Level 1 Commands (from the display Datasheet) */
#define ILI9488_CMD_NOP                             0x00
#define ILI9488_CMD_SOFTWARE_RESET                  0x01
#define ILI9488_CMD_READ_DISP_ID                    0x04
#define ILI9488_CMD_READ_ERROR_DSI                  0x05
#define ILI9488_CMD_READ_DISP_STATUS                0x09
#define ILI9488_CMD_READ_DISP_POWER_MODE            0x0A
#define ILI9488_CMD_READ_DISP_MADCTRL               0x0B
#define ILI9488_CMD_READ_DISP_PIXEL_FORMAT          0x0C
#define ILI9488_CMD_READ_DISP_IMAGE_MODE            0x0D
#define ILI9488_CMD_READ_DISP_SIGNAL_MODE           0x0E
#define ILI9488_CMD_READ_DISP_SELF_DIAGNOSTIC       0x0F
#define ILI9488_CMD_ENTER_SLEEP_MODE                0x10
#define ILI9488_CMD_SLEEP_OUT                       0x11
#define ILI9488_CMD_PARTIAL_MODE_ON                 0x12
#define ILI9488_CMD_NORMAL_DISP_MODE_ON             0x13
#define ILI9488_CMD_DISP_INVERSION_OFF              0x20
#define ILI9488_CMD_DISP_INVERSION_ON               0x21
#define ILI9488_CMD_PIXEL_OFF                       0x22
#define ILI9488_CMD_PIXEL_ON                        0x23
#define ILI9488_CMD_DISPLAY_OFF                     0x28
#define ILI9488_CMD_DISPLAY_ON                      0x29
#define ILI9488_CMD_COLUMN_ADDRESS_SET              0x2A
#define ILI9488_CMD_PAGE_ADDRESS_SET                0x2B
#define ILI9488_CMD_MEMORY_WRITE                    0x2C
#define ILI9488_CMD_MEMORY_READ                     0x2E
#define ILI9488_CMD_PARTIAL_AREA                    0x30
#define ILI9488_CMD_VERT_SCROLL_DEFINITION          0x33
#define ILI9488_CMD_TEARING_EFFECT_LINE_OFF         0x34
#define ILI9488_CMD_TEARING_EFFECT_LINE_ON          0x35
#define ILI9488_CMD_MEMORY_ACCESS_CONTROL           0x36
#define ILI9488_CMD_VERT_SCROLL_START_ADDRESS       0x37
#define ILI9488_CMD_IDLE_MODE_OFF                   0x38
#define ILI9488_CMD_IDLE_MODE_ON                    0x39
#define ILI9488_CMD_COLMOD_PIXEL_FORMAT_SET         0x3A
#define ILI9488_CMD_WRITE_MEMORY_CONTINUE           0x3C
#define ILI9488_CMD_READ_MEMORY_CONTINUE            0x3E
#define ILI9488_CMD_SET_TEAR_SCANLINE               0x44
#define ILI9488_CMD_GET_SCANLINE                    0x45
#define ILI9488_CMD_WRITE_DISPLAY_BRIGHTNESS        0x51
#define ILI9488_CMD_READ_DISPLAY_BRIGHTNESS         0x52
#define ILI9488_CMD_WRITE_CTRL_DISPLAY              0x53
#define ILI9488_CMD_READ_CTRL_DISPLAY               0x54
#define ILI9488_CMD_WRITE_CONTENT_ADAPT_BRIGHTNESS  0x55
#define ILI9488_CMD_READ_CONTENT_ADAPT_BRIGHTNESS   0x56
#define ILI9488_CMD_WRITE_MIN_CAB_LEVEL             0x5E
#define ILI9488_CMD_READ_MIN_CAB_LEVEL              0x5F
#define ILI9488_CMD_READ_ABC_SELF_DIAG_RES          0x68
#define ILI9488_CMD_READ_ID1                        0xDA
#define ILI9488_CMD_READ_ID2                        0xDB
#define ILI9488_CMD_READ_ID3                        0xDC

/* Level 2 Commands (from the display Datasheet) */
#define ILI9488_CMD_INTERFACE_MODE_CONTROL          0xB0
#define ILI9488_CMD_FRAME_RATE_CONTROL_NORMAL       0xB1
#define ILI9488_CMD_FRAME_RATE_CONTROL_IDLE_8COLOR  0xB2
#define ILI9488_CMD_FRAME_RATE_CONTROL_PARTIAL      0xB3
#define ILI9488_CMD_DISPLAY_INVERSION_CONTROL       0xB4
#define ILI9488_CMD_BLANKING_PORCH_CONTROL          0xB5
#define ILI9488_CMD_DISPLAY_FUNCTION_CONTROL        0xB6
#define ILI9488_CMD_ENTRY_MODE_SET                  0xB7
#define ILI9488_CMD_BACKLIGHT_CONTROL_1             0xB9
#define ILI9488_CMD_BACKLIGHT_CONTROL_2             0xBA
#define ILI9488_CMD_HS_LANES_CONTROL                0xBE
#define ILI9488_CMD_POWER_CONTROL_1                 0xC0
#define ILI9488_CMD_POWER_CONTROL_2                 0xC1
#define ILI9488_CMD_POWER_CONTROL_NORMAL_3          0xC2
#define ILI9488_CMD_POWER_CONTROL_IDEL_4            0xC3
#define ILI9488_CMD_POWER_CONTROL_PARTIAL_5         0xC4
#define ILI9488_CMD_VCOM_CONTROL_1                  0xC5
#define ILI9488_CMD_CABC_CONTROL_1                  0xC6
#define ILI9488_CMD_CABC_CONTROL_2                  0xC8
#define ILI9488_CMD_CABC_CONTROL_3                  0xC9
#define ILI9488_CMD_CABC_CONTROL_4                  0xCA
#define ILI9488_CMD_CABC_CONTROL_5                  0xCB
#define ILI9488_CMD_CABC_CONTROL_6                  0xCC
#define ILI9488_CMD_CABC_CONTROL_7                  0xCD
#define ILI9488_CMD_CABC_CONTROL_8                  0xCE
#define ILI9488_CMD_CABC_CONTROL_9                  0xCF
#define ILI9488_CMD_NVMEM_WRITE                     0xD0
#define ILI9488_CMD_NVMEM_PROTECTION_KEY            0xD1
#define ILI9488_CMD_NVMEM_STATUS_READ               0xD2
#define ILI9488_CMD_READ_ID4                        0xD3
#define ILI9488_CMD_ADJUST_CONTROL_1                0xD7
#define ILI9488_CMD_READ_ID_VERSION                 0xD8
#define ILI9488_CMD_POSITIVE_GAMMA_CORRECTION       0xE0
#define ILI9488_CMD_NEGATIVE_GAMMA_CORRECTION       0xE1
#define ILI9488_CMD_DIGITAL_GAMMA_CONTROL_1         0xE2
#define ILI9488_CMD_DIGITAL_GAMMA_CONTROL_2         0xE3
#define ILI9488_CMD_SET_IMAGE_FUNCTION              0xE9
#define ILI9488_CMD_ADJUST_CONTROL_2                0xF2
#define ILI9488_CMD_ADJUST_CONTROL_3                0xF7
#define ILI9488_CMD_ADJUST_CONTROL_4                0xF8
#define ILI9488_CMD_ADJUST_CONTROL_5                0xF9
#define ILI9488_CMD_SPI_READ_SETTINGS               0xFB
#define ILI9488_CMD_ADJUST_CONTROL_6                0xFC
#define ILI9488_CMD_ADJUST_CONTROL_7                0xFF

#define get_0b_to_8b(x)             (((union_type*)&(x))->byte.byte_8)
#define get_8b_to_16b(x)            (((union_type*)&(x))->byte.byte_l6)
#define get_16b_to_24b(x)           (((union_type*)&(x))->byte.byte_24)
#define get_24b_to_32b(x)           (((union_type*)&(x))->byte.byte_32)


uint32_t PV035HV_CINA5007_get_chip_id(void);
void PV035HV_CINA5007_set_cursor_position(uint16_t x, uint16_t y);
void PV035HV_CINA5007_write_register(uint8_t u8_cmd, const uint8_t * kpu8_data, uint32_t u32_size);
void PV035HV_CINA5007_set_window(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void PV035HV_CINA5007_fill(uint16_t u16_color);


/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void PV035HV_CINA5007_init(PV035HV_CINA5007_mode_t mode)
{
	ASSERT(mode < PV035HV_CINA5007_MODE_NUM_MODES);

	uint8_t param;

	switch (mode)
	{
		case PV035HV_CINA5007_MODE_SPI:
		{
			IO_config_pin_direction(PV035HV_CINA5007_NRESET_PIN, IO_DIRECTION_OUTPUT);
			IO_config_pin_direction(PV035HV_CINA5007_RS_PIN, IO_DIRECTION_OUTPUT);

			// Assert NRESET and stabilize
			IO_set_pin(PV035HV_CINA5007_NRESET_PIN, IO_PIN_STATE_LOW);
			CHRONO_delay_ms(200);

			// Deassert NRESET and stabilize
			IO_set_pin(PV035HV_CINA5007_NRESET_PIN, IO_PIN_STATE_HIGH);
			CHRONO_delay_ms(200);

			SPI_init(SPI_CHANNEL_DISPLAY);



			PV035HV_CINA5007_write_register(ILI9488_CMD_SOFTWARE_RESET, 0x0000, 0);
			CHRONO_delay_ms(200);

			PV035HV_CINA5007_write_register(ILI9488_CMD_SLEEP_OUT, 0x0000, 0);
			CHRONO_delay_ms(250);
			


			// /** make it tRGB and reverse the column order */
			// param = 0x48;
			// PV035HV_CINA5007_write_register(ILI9488_CMD_MEMORY_ACCESS_CONTROL, &param, 1);
			// CHRONO_delay_ms(100);

			// param = 0x04;
			// PV035HV_CINA5007_write_register(ILI9488_CMD_CABC_CONTROL_9, &param, 1);
			// CHRONO_delay_ms(100);

			param = 0x05; // RGB565, 2 bytes/ pixel
			PV035HV_CINA5007_write_register(ILI9488_CMD_COLMOD_PIXEL_FORMAT_SET, &param, 1);
			CHRONO_delay_ms(100);

			PV035HV_CINA5007_write_register(ILI9488_CMD_NORMAL_DISP_MODE_ON, 0, 0);
			CHRONO_delay_ms(100);

			PV035HV_CINA5007_write_register(ILI9488_CMD_DISPLAY_ON, 0, 0);
			CHRONO_delay_ms(100);

			PV035HV_CINA5007_fill(0xFEFE);

			// param = 0x48;
			// PV035HV_CINA5007_write_register(ILI9488_CMD_MEMORY_ACCESS_CONTROL, &param, 1);
			// CHRONO_delay_ms(100);

			// PV035HV_CINA5007_set_window(0, 0,320,480);
			// PV035HV_CINA5007_fill(0x00FF00);
			// PV035HV_CINA5007_set_cursor_position(0, 0);
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

void PV035HV_CINA5007_write_register(uint8_t u8_cmd, const uint8_t * kpu8_data, uint32_t u32_size)
{
	PV035HV_CINA5007_write_command(u8_cmd);

	for (uint32_t i = 0; i < u32_size; ++i)
	{
		PV035HV_CINA5007_write_data(kpu8_data[i]);
	}
}

void PV035HV_CINA5007_set_window(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	uint16_t col_start  = x;
	uint16_t col_end    = x + width - 1;
	uint16_t row_start  = y;
	uint16_t row_end    = y + height - 1;

	uint8_t col_addr_data[4] = {
		(uint8_t)(col_start >> 8),
		(uint8_t)(col_start & 0xFF),
		(uint8_t)(col_end >> 8),
		(uint8_t)(col_end & 0xFF)
	};

	uint8_t row_addr_data[4] = {
		(uint8_t)(row_start >> 8),
		(uint8_t)(row_start & 0xFF),
		(uint8_t)(row_end >> 8),
		(uint8_t)(row_end & 0xFF)
	};

	// Set column address
	PV035HV_CINA5007_write_register(0x2A, col_addr_data, 4); // ILI9488_CMD_COLUMN_ADDRESS_SET

	// Set row (page) address
	PV035HV_CINA5007_write_register(0x2B, row_addr_data, 4); // ILI9488_CMD_PAGE_ADDRESS_SET
}

void PV035HV_CINA5007_set_cursor_position(uint16_t x, uint16_t y)
{
	uint8_t col_data[4] = {
		(uint8_t)(x >> 8),
		(uint8_t)(x & 0xFF),
		(uint8_t)(x >> 8),
		(uint8_t)(x & 0xFF)
	};

	uint8_t row_data[4] = {
		(uint8_t)(y >> 8),
		(uint8_t)(y & 0xFF),
		(uint8_t)(y >> 8),
		(uint8_t)(y & 0xFF)
	};

	// Set column address (X)
	PV035HV_CINA5007_write_register(0x2A, col_data, 4);

	// Set row (page) address (Y)
	PV035HV_CINA5007_write_register(0x2B, row_data, 4);
}

uint32_t PV035HV_CINA5007_get_chip_id(void)
{
	uint8_t pu8_id_bytes[3] = {0};

	// Set D/C LOW → command
	IO_set_pin(PV035HV_CINA5007_RS_PIN, IO_PIN_STATE_LOW);
	SPI_transfer(SPI_CHANNEL_DISPLAY, 0x04); // Read Display ID

	// Set D/C HIGH → data (read phase)
	IO_set_pin(PV035HV_CINA5007_RS_PIN, IO_PIN_STATE_HIGH);

	// Read 3 ID bytes (MISO response)
	pu8_id_bytes[0] = SPI_transfer(SPI_CHANNEL_DISPLAY, 0x00); // dummy byte → gets response
	pu8_id_bytes[1] = SPI_transfer(SPI_CHANNEL_DISPLAY, 0x00); // "
	pu8_id_bytes[2] = SPI_transfer(SPI_CHANNEL_DISPLAY, 0x00); // "

	// Combine into 24-bit ID (optional)
	return ((uint32_t)pu8_id_bytes[0] << 16) |
		   ((uint32_t)pu8_id_bytes[1] << 8)  |
		   (uint32_t)pu8_id_bytes[2];
}


void PV035HV_CINA5007_fill(uint16_t u16_color)
{
	uint8_t u8_high = (u16_color >> 8) & 0xFF;
	uint8_t u8_low = u16_color & 0xFF;

	PV035HV_CINA5007_set_cursor_position(0, 0);
	PV035HV_CINA5007_write_command(ILI9488_CMD_MEMORY_WRITE);

	for (uint32_t i = 0; i < 320 * 480; i++)
	{
		PV035HV_CINA5007_write_data(u8_low);
		PV035HV_CINA5007_write_data(u8_high);
	}
}