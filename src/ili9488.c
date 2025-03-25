#include "ili9488.h"
#include "utils.h"
#include "io.h"
#include "spi.h"
#include "chrono.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define ILI9488_NRESET_PIN IO_PIN_ID_PA05
#define ILI9488_RS_PIN IO_PIN_ID_PA04

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

typedef union PACKED _ILI9488_status
{
	struct _ILI9488_status_bits
	{
		// Byte 3: D31–D24
		uint8_t booster_on         : 1; // D31
		uint8_t row_address_order  : 1; // D30
		uint8_t col_address_order  : 1; // D29
		uint8_t row_col_exchange   : 1; // D28
		uint8_t vertical_refresh   : 1; // D27
		uint8_t rgb_bgr_order      : 1; // D26
		uint8_t horizontal_refresh : 1; // D25
		uint8_t reserved0          : 1; // D24

		// Byte 2: D23–D16
		uint8_t reserved1          : 1; // D23
		uint8_t pixel_format0      : 1; // D22
		uint8_t pixel_format1      : 1; // D21
		uint8_t pixel_format2      : 1; // D20
		uint8_t idle_mode          : 1; // D19
		uint8_t partial_mode       : 1; // D18
		uint8_t sleep_out          : 1; // D17
		uint8_t normal_mode        : 1; // D16

		// Byte 1: D15–D8
		uint8_t vertical_scroll    : 1; // D15
		uint8_t reserved2          : 1; // D14
		uint8_t inversion          : 1; // D13
		uint8_t reserved3          : 2; // D12–D11
		uint8_t display_on         : 1; // D10
		uint8_t tearing_line       : 1; // D9
		uint8_t gamma_curve        : 1; // D8 — LSB of gamma bits

		// Byte 0: D7–D0
		uint8_t gamma_curve1       : 1; // D7
		uint8_t gamma_curve2       : 1; // D6
		uint8_t tearing_mode       : 1; // D5
		uint8_t reserved4          : 5; // D4–D0
	} bits;
	uint32_t u32_raw;
} ILI9488_status_t;



uint32_t ILI9488_get_chip_id(void);
void ILI9488_set_cursor_position(uint16_t x, uint16_t y);
void ILI9488_write_register(uint8_t u8_cmd, const uint8_t * kpu8_data, uint32_t u32_size);
void ILI9488_set_window(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void ILI9488_fill(uint16_t u16_color);
void ILI9488_read_display_status(void);

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
			// Initialize display SPI channel
			SPI_init(SPI_CHANNEL_DISPLAY);

			IO_config_pin_direction(ILI9488_NRESET_PIN, IO_DIRECTION_OUTPUT);
			IO_config_pin_direction(ILI9488_RS_PIN, IO_DIRECTION_OUTPUT);

			// Assert NRESET and stabilize
			IO_set_pin(ILI9488_NRESET_PIN, IO_PIN_STATE_LOW);
			CHRONO_delay_ms(200);

			// Deassert NRESET and stabilize
			IO_set_pin(ILI9488_NRESET_PIN, IO_PIN_STATE_HIGH);
			CHRONO_delay_ms(200);


			ILI9488_write_register(ILI9488_CMD_SOFTWARE_RESET, 0x0000, 0);
			CHRONO_delay_ms(200);

			ILI9488_write_register(ILI9488_CMD_SLEEP_OUT, 0x0000, 0);
			CHRONO_delay_ms(250);

			ILI9488_write_register(ILI9488_CMD_NORMAL_DISP_MODE_ON, 0, 0);
			CHRONO_delay_ms(200);

			// param = 0x04;
			// ILI9488_write_register(ILI9488_CMD_CABC_CONTROL_9, &param, 1);
			// CHRONO_delay_ms(200);
			
			// param = 0x05;
			// ILI9488_write_register(ILI9488_CMD_COLMOD_PIXEL_FORMAT_SET, &param, 1);
			// CHRONO_delay_ms(200);

			// param = 0x48;
			// ILI9488_write_register(ILI9488_CMD_MEMORY_ACCESS_CONTROL, &param, 1);
			// CHRONO_delay_ms(200);

			ILI9488_write_register(ILI9488_CMD_DISPLAY_ON, 0, 0); // 
			CHRONO_delay_ms(200);

			// ILI9488_write_register(ILI9488_CMD_PIXEL_ON, 0, 0);

			// ILI9488_set_window(0, 0, 320, 480);
			// ILI9488_set_cursor_position(0, 0);

			// ILI9488_fill(0x00FF00);
			ILI9488_read_display_status();
		}
	}
}


			// ILI9488_fill(0xFEFE);

			// param = 0x48;
			// ILI9488_write_register(ILI9488_CMD_MEMORY_ACCESS_CONTROL, &param, 1);
			// CHRONO_delay_ms(100);


			// /** make it tRGB and reverse the column order */
			// param = 0x48;
			// ILI9488_write_register(ILI9488_CMD_MEMORY_ACCESS_CONTROL, &param, 1);
			// CHRONO_delay_ms(200);







			// param = 0xFF;
			// ILI9488_write_register(ILI9488_CMD_WRITE_DISPLAY_BRIGHTNESS, &param, 1);
			// CHRONO_delay_ms(200);


void ILI9488_write_command(uint8_t u8_cmd)
{
	IO_set_pin(ILI9488_RS_PIN, IO_PIN_STATE_LOW); // Set D/C low for command
	SPI_transfer(SPI_CHANNEL_DISPLAY, u8_cmd);
}

void ILI9488_write_data(uint8_t u8_data)
{
	IO_set_pin(ILI9488_RS_PIN, IO_PIN_STATE_HIGH); // Set D/C high for data
	SPI_transfer(SPI_CHANNEL_DISPLAY, u8_data);
}

void ILI9488_write_register(uint8_t u8_cmd, const uint8_t * kpu8_data, uint32_t u32_size)
{
	ILI9488_write_command(u8_cmd);

	for (uint32_t i = 0; i < u32_size; i++)
	{
		ILI9488_write_data(kpu8_data[i]);
	}
}

void ILI9488_set_window(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	uint16_t col_start  = x;
	uint16_t col_end    = x + width - 1;
	uint16_t row_start  = y;
	uint16_t row_end    = y + height - 1;

	uint8_t col_addr_data[4] =
	{
		(uint8_t)(col_start >> 8),
		(uint8_t)(col_start & 0xFF),
		(uint8_t)(col_end >> 8),
		(uint8_t)(col_end & 0xFF)
	};

	uint8_t row_addr_data[4] =
	{
		(uint8_t)(row_start >> 8),
		(uint8_t)(row_start & 0xFF),
		(uint8_t)(row_end >> 8),
		(uint8_t)(row_end & 0xFF)
	};

	// Set column address
	ILI9488_write_register(ILI9488_CMD_COLUMN_ADDRESS_SET, col_addr_data, 4);
	CHRONO_delay_ms(120);

	// Set row (page) address
	ILI9488_write_register(ILI9488_CMD_PAGE_ADDRESS_SET, row_addr_data, 4);
	CHRONO_delay_ms(120);
}

void ILI9488_set_cursor_position(uint16_t x, uint16_t y)
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
	ILI9488_write_register(0x2A, col_data, 4);

	// Set row (page) address (Y)
	ILI9488_write_register(0x2B, row_data, 4);
}

uint32_t ILI9488_get_chip_id(void)
{
	uint8_t pu8_id_bytes[3] = {0};

	// Set D/C LOW → command
	IO_set_pin(ILI9488_RS_PIN, IO_PIN_STATE_LOW);
	SPI_transfer(SPI_CHANNEL_DISPLAY, 0x04); // Read Display ID

	// Set D/C HIGH → data (read phase)
	IO_set_pin(ILI9488_RS_PIN, IO_PIN_STATE_HIGH);

	// Read 3 ID bytes (MISO response)
	pu8_id_bytes[0] = SPI_transfer(SPI_CHANNEL_DISPLAY, 0x00); // dummy byte → gets response
	pu8_id_bytes[1] = SPI_transfer(SPI_CHANNEL_DISPLAY, 0x00); // "
	pu8_id_bytes[2] = SPI_transfer(SPI_CHANNEL_DISPLAY, 0x00); // "

	// Combine into 24-bit ID (optional)
	return ((uint32_t)pu8_id_bytes[0] << 16) |
		   ((uint32_t)pu8_id_bytes[1] << 8)  |
		   (uint32_t)pu8_id_bytes[2];
}


void ILI9488_fill(uint16_t u16_color)
{
	uint8_t u8_high = (u16_color >> 8) & 0xFF;
	uint8_t u8_low = u16_color & 0xFF;

	ILI9488_set_cursor_position(0, 0);
	ILI9488_write_command(ILI9488_CMD_MEMORY_WRITE);

	for (uint32_t i = 0; i < 320 * 480; i++)
	{
		ILI9488_write_data(u8_low);
		ILI9488_write_data(u8_high);
	}
}

void ILI9488_read_display_status(void)
{
	ILI9488_status_t status;

	uint8_t * pu8_status = &status.u32_raw;

	ILI9488_write_register(ILI9488_CMD_READ_DISP_STATUS, 0, 0);

	IO_set_pin(ILI9488_RS_PIN, IO_PIN_STATE_HIGH);

	SPI_transfer(SPI_CHANNEL_DISPLAY, 0x00); // dummy byte

	// Read display status (D31 down to D0)
	pu8_status[0] = SPI_transfer(SPI_CHANNEL_DISPLAY, 0x00); // D31–D24
	pu8_status[1] = SPI_transfer(SPI_CHANNEL_DISPLAY, 0x00); // D23–D16
	pu8_status[2] = SPI_transfer(SPI_CHANNEL_DISPLAY, 0x00); // D15–D8
	pu8_status[3] = SPI_transfer(SPI_CHANNEL_DISPLAY, 0x00); // D7–D0

	SHELL_printf("booster_on : %u\n", status.bits.booster_on);
	SHELL_printf("row_address_order : %u\n", status.bits.row_address_order);
	SHELL_printf("col_address_order : %u\n", status.bits.col_address_order);
	SHELL_printf("row_col_exchange : %u\n", status.bits.row_col_exchange);
	SHELL_printf("vertical_refresh : %u\n", status.bits.vertical_refresh);
	SHELL_printf("rgb_bgr_order : %u\n", status.bits.rgb_bgr_order);
	SHELL_printf("horizontal_refresh : %u\n", status.bits.horizontal_refresh);
	SHELL_printf("pixel_format0 : %u\n", status.bits.pixel_format0);
	SHELL_printf("pixel_format1 : %u\n", status.bits.pixel_format1);
	SHELL_printf("pixel_format2 : %u\n", status.bits.pixel_format2);
	SHELL_printf("idle_mode : %u\n", status.bits.idle_mode);
	SHELL_printf("partial_mode : %u\n", status.bits.partial_mode);
	SHELL_printf("sleep_out : %u\n", status.bits.sleep_out);
	SHELL_printf("normal_mode : %u\n", status.bits.normal_mode);
	SHELL_printf("vertical_scroll : %u\n", status.bits.vertical_scroll);
	SHELL_printf("inversion : %u\n", status.bits.inversion);
	SHELL_printf("display_on : %u\n", status.bits.display_on);
	SHELL_printf("tearing_line : %u\n", status.bits.tearing_line);
	SHELL_printf("gamma_curve : %u\n", status.bits.gamma_curve);
	SHELL_printf("gamma_curve1 : %u\n", status.bits.gamma_curve1);
	SHELL_printf("gamma_curve2 : %u\n", status.bits.gamma_curve2);
	SHELL_printf("tearing_mode : %u\n", status.bits.tearing_mode);
}
