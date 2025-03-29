#include "ili9488.h"
#include "utils.h"
#include "io.h"
#include "spi.h"
#include "chrono.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define ILI9488_NRESET_PIN 	IO_PIN_ID_PA05
#define ILI9488_RS_PIN 		IO_PIN_ID_PA04
#define ILI9488_CSX_PIN 	IO_PIN_ID_PA03

#define ILI9488_DISPLAY_HEIGHT 						(480U)
#define ILI9488_DISPLAY_WIDTH 						(320U)

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



#define ILI9488_DISPLAY_ON_MASK 			(1UL << (10UL - 1))
#define ILI9488_INVERSION_ON_MASK 			(1UL << (13UL - 1))
#define ILI9488_SLEEP_OUT_MASK 				(1UL << (17UL - 1))
#define ILI9488_IDLE_MODE_ON_MASK 			(1UL << (19UL - 1))
#define ILI9488_INTERFACE_PIX_FMT_0_MASK 	(1UL << (20UL - 1))
#define ILI9488_INTERFACE_PIX_FMT_1_MASK 	(1UL << (21UL - 1))
#define ILI9488_INTERFACE_PIX_FMT_2_MASK 	(1UL << (22UL - 1))
#define ILI9488_ROW_COL_EXCHANGE_MASK 		(1UL << (28UL - 1))
#define ILI9488_COL_ADDRESS_ORDER_MASK 		(1UL << (29UL - 1))



uint32_t ILI9488_get_chip_id(void);
void ILI9488_set_cursor_position(uint16_t x, uint16_t y);
void ILI9488_write_register(uint8_t u8_cmd, const uint8_t * kpu8_data, uint32_t u32_size);
void ILI9488_set_window(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void ILI9488_fill(uint16_t u16_color);
void ILI9488_read_display_status(void);


void ILI9488_write_command_raw(uint8_t cmd);
void ILI9488_fill_one_pixel(uint16_t u16_color);
void ILI9488_write_pixel_rgb888(uint8_t r, uint8_t g, uint8_t b);

static void ILI9488_write_command(uint8_t u8_cmd);
static void ILI9488_write_data(const uint8_t * ku8_data, uint32_t u32_size);
static void ILI9488_read_display_madctrl(void);
static void ILI9488_read_display_colmod(void);

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void ILI9488_init(ILI9488_mode_t mode)
{
	ASSERT(mode < ILI9488_MODE_NUM_MODES);

	uint8_t param;
	UNUSED(param);

	switch (mode)
	{
		case ILI9488_MODE_SPI:
		{
			SPI_init(SPI_CHANNEL_DISPLAY);

			IO_config_pin_direction(ILI9488_NRESET_PIN, IO_DIRECTION_OUTPUT);
			IO_config_pin_direction(ILI9488_RS_PIN, IO_DIRECTION_OUTPUT);
			IO_config_pin_direction(ILI9488_CSX_PIN, IO_DIRECTION_OUTPUT);

			IO_set_pin(ILI9488_CSX_PIN, IO_PIN_STATE_HIGH);

			// Assert NRESET and stabilize
			IO_set_pin(ILI9488_NRESET_PIN, IO_PIN_STATE_LOW);
			CHRONO_delay_ms(200);

			// Deassert NRESET and stabilize
			IO_set_pin(ILI9488_NRESET_PIN, IO_PIN_STATE_HIGH);
			CHRONO_delay_ms(200);

			// Hold CS line low
			IO_set_pin(ILI9488_CSX_PIN, IO_PIN_STATE_LOW);

			ILI9488_write_command(ILI9488_CMD_SOFTWARE_RESET);
			CHRONO_delay_ms(200);

			ILI9488_write_command(ILI9488_CMD_SLEEP_OUT);
			CHRONO_delay_ms(200);

			// RGB888 (this is the only mode SPI supports afaik)
			param = 0x06;
			ILI9488_write_command(ILI9488_CMD_COLMOD_PIXEL_FORMAT_SET);
			ILI9488_write_data(&param, 1);
			CHRONO_delay_ms(200);

			// Orientation
			param = 0x48; // landscape
			// param = 0xE8; // portrait
			ILI9488_write_command(ILI9488_CMD_MEMORY_ACCESS_CONTROL);
			ILI9488_write_data(&param, 1);
			CHRONO_delay_ms(200);

			// Exit IDLE mode
			ILI9488_write_command(ILI9488_CMD_NORMAL_DISP_MODE_ON);
			CHRONO_delay_ms(200);

			// Optional brightness / CABC
			param = 0x04;
			ILI9488_write_command(ILI9488_CMD_CABC_CONTROL_9);
			ILI9488_write_data(&param, 1);
			CHRONO_delay_ms(200);

			// Display ON
			ILI9488_write_command(ILI9488_CMD_DISPLAY_ON);
			CHRONO_delay_ms(200);

			// ILI9488_set_window(0, 0, ILI9488_DISPLAY_WIDTH, ILI9488_DISPLAY_HEIGHT);

			// // panel fill (w * h * bit depth = 460800 in RGB888....)
			// for (uint8_t i = 0; i < ILI9488_DISPLAY_WIDTH * ILI9488_DISPLAY_HEIGHT; i++) 
			// {
			// 	ILI9488_write_pixel_rgb888(0xFF, 0x00, 0x00);
			// }

			ILI9488_read_display_status();
			ILI9488_read_display_madctrl();
			ILI9488_read_display_colmod();
		}
	}
}

static void ILI9488_write_command(uint8_t u8_cmd)
{
	IO_set_pin(ILI9488_RS_PIN, IO_PIN_STATE_LOW); // Set D/C low for command
	// IO_set_pin(ILI9488_CSX_PIN, IO_PIN_STATE_LOW);
	SPI_transfer(SPI_CHANNEL_DISPLAY, u8_cmd);
	// IO_set_pin(ILI9488_RS_PIN, IO_PIN_STATE_HIGH); // Reset RS
	// IO_set_pin(ILI9488_CSX_PIN, IO_PIN_STATE_HIGH);
}

static void ILI9488_write_data(const uint8_t * ku8_data, uint32_t u32_size)
{
	if (0 == u32_size)
	{
		return;
	}

	IO_set_pin(ILI9488_RS_PIN, IO_PIN_STATE_HIGH); // Set D/C high for data
	// IO_set_pin(ILI9488_CSX_PIN, IO_PIN_STATE_LOW);

	for (uint32_t i = 0; i < u32_size; i++)
	{
		SPI_transfer(SPI_CHANNEL_DISPLAY, ku8_data[i]);
	}

	// IO_set_pin(ILI9488_RS_PIN, IO_PIN_STATE_LOW); // Set D/C high for data
	// IO_set_pin(ILI9488_CSX_PIN, IO_PIN_STATE_HIGH);
}

void ILI9488_set_window(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	uint8_t data[4];

	// Set column address (0x2A)
	data[0] = x >> 8;
	data[1] = x & 0xFF;
	data[2] = (x + w - 1) >> 8;
	data[3] = (x + w - 1) & 0xFF;
	ILI9488_write_command(0x2A);
	ILI9488_write_data(data, 4);

	// Set row/page address (0x2B)
	data[0] = y >> 8;
	data[1] = y & 0xFF;
	data[2] = (y + h - 1) >> 8;
	data[3] = (y + h - 1) & 0xFF;
	ILI9488_write_command(0x2B);
	ILI9488_write_data(data, 4);

	// Memory write (0x2C)
	ILI9488_write_command(0x2C);
}

void ILI9488_write_pixel_rgb888(uint8_t r, uint8_t g, uint8_t b)
{
	uint8_t pixel[3] = { r, g, b };
	ILI9488_write_data(pixel, 3);
}


void ILI9488_read_display_status(void)
{
	uint32_t u32_status;
	uint8_t * pu8_status = (uint8_t *)&u32_status;

	// IO_set_pin(ILI9488_CSX_PIN, IO_PIN_STATE_LOW);

	IO_set_pin(ILI9488_RS_PIN, IO_PIN_STATE_LOW);
	SPI_transfer(SPI_CHANNEL_DISPLAY, ILI9488_CMD_READ_DISP_STATUS);

	// switch to data mode and read
	IO_set_pin(ILI9488_RS_PIN, IO_PIN_STATE_HIGH);
	// SPI_transfer(SPI_CHANNEL_DISPLAY, 0x00); // Dummy byte (ignored)

	pu8_status[3] = SPI_transfer(SPI_CHANNEL_DISPLAY, 0xFF); // D31–D24
	pu8_status[2] = SPI_transfer(SPI_CHANNEL_DISPLAY, 0xFF); // D23–D16
	pu8_status[1] = SPI_transfer(SPI_CHANNEL_DISPLAY, 0xFF); // D15–D8
	pu8_status[0] = SPI_transfer(SPI_CHANNEL_DISPLAY, 0xFF); // D7–D0

	// IO_set_pin(ILI9488_CSX_PIN, IO_PIN_STATE_HIGH);

	SHELL_printf("u32_status: 0x%08X\n", u32_status);
	SHELL_printf("DISPLAY_ON: %u\n", u32_status & ILI9488_DISPLAY_ON_MASK ? 1: 0);
	SHELL_printf("INVERSION_ON: %u\n", u32_status & ILI9488_INVERSION_ON_MASK ? 1: 0);
	SHELL_printf("SLEEP_OUT: %u\n", u32_status & ILI9488_SLEEP_OUT_MASK ? 1: 0);
	SHELL_printf("IDLE_MODE_ON: %u\n", u32_status & ILI9488_IDLE_MODE_ON_MASK ? 1: 0);
	SHELL_printf("INTERFACE_PIX_FMT_0: %u\n", u32_status & ILI9488_INTERFACE_PIX_FMT_0_MASK ? 1: 0);
	SHELL_printf("INTERFACE_PIX_FMT_1: %u\n", u32_status & ILI9488_INTERFACE_PIX_FMT_1_MASK ? 1: 0);
	SHELL_printf("INTERFACE_PIX_FMT_2: %u\n", u32_status & ILI9488_INTERFACE_PIX_FMT_2_MASK ? 1: 0);
	SHELL_printf("ROW_COL_EXCHANGE: %u\n", u32_status & ILI9488_ROW_COL_EXCHANGE_MASK ? 1: 0);
	SHELL_printf("COL_ADDRESS_ORDER: %u\n", u32_status & ILI9488_COL_ADDRESS_ORDER_MASK ? 1: 0);
}

// fair enough
static void ILI9488_read_display_madctrl(void)
{
	uint8_t u8_data;

	ILI9488_write_command(ILI9488_CMD_READ_DISP_MADCTRL);

	IO_set_pin(ILI9488_RS_PIN, IO_PIN_STATE_HIGH);

	SPI_transfer(SPI_CHANNEL_DISPLAY, 0xFF); // Dummy

	u8_data = SPI_transfer(SPI_CHANNEL_DISPLAY, 0xFF);

	SHELL_printf("MADCTRL (raw): %u\n", u8_data);
}

static void ILI9488_read_display_colmod(void)
{
	uint8_t u8_data;

	ILI9488_write_command(ILI9488_CMD_READ_DISP_PIXEL_FORMAT);

	IO_set_pin(ILI9488_RS_PIN, IO_PIN_STATE_HIGH);

	u8_data = SPI_transfer(SPI_CHANNEL_DISPLAY, 0xFF); // dummy

	SHELL_printf("COLMOD (raw): %u\n", u8_data);

	u8_data = SPI_transfer(SPI_CHANNEL_DISPLAY, 0xFF);

	SHELL_printf("COLMOD (raw): %u\n", u8_data);
}
