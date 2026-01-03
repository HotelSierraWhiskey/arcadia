#include "fw_update.h"
#include "bootloader.h"
#include "spi.h"
#include "sd.h"
#include "fsif.h"

bool FW_UPDATE_init (void)
{
	bool b_res = false;

	SPI_init(SPI_CHANNEL_SD_CARD);

	if (SD_card_init())
	{
		if (FSIF_fs_init())
		{
			b_res = true;
		}
		else
		{
			BOOT_LOG_DBG("Failed to mount file system\n");
		}
	}
	else
	{
		BOOT_LOG_DBG("Failed to initialize SD card\n");
	}

	return b_res;
}
