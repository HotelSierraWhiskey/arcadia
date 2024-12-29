#include "fsif.h"
#include "diskio.h"
#include "sd.h"
#include "utils.h"
#include "shell.h"
#include "arcadia.h"
#include "chrono.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define FSIF_LOG_DBG(fmt, ...)   			SHELL_printf("\r%-10s" fmt, "[FSIF]", ##__VA_ARGS__)
#define FSIF_LOG_WARN(fmt, ...)   		SHELL_PRINT_WARNING("\r%-10s" fmt, "[FSIF]", ##__VA_ARGS__)

typedef struct _FSIF_info
{
	FATFS				fs;
	SemaphoreHandle_t	semaphore;
	StaticSemaphore_t	semaphore_buffer;
} FSIF_info_t;

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

static FSIF_info_t fsif_info;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

DSTATUS disk_initialize(BYTE pdrv)
{
	UNUSED(pdrv);

	return 0;
}

DSTATUS disk_status(BYTE pdrv)
{
	UNUSED(pdrv);

	if (SD_is_initialized())
	{
		return 0;
	}

	return STA_NOINIT;
}

DRESULT disk_read(BYTE pdrv, BYTE* buff, LBA_t sector, UINT count)
{
	UNUSED(pdrv);

	for (uint8_t i = 0; i < count; i++)
	{
		SD_read_block((sector + i) * SD_BLOCK_SIZE, buff + (i * SD_BLOCK_SIZE));
	}

	return RES_OK;
}

DRESULT disk_write(BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count)
{
	UNUSED(pdrv);

	for (uint8_t i = 0; i < count; i++)
	{
		SD_write_block((sector + i) * SD_BLOCK_SIZE, buff + (i * SD_BLOCK_SIZE));
	}

	return RES_OK;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff)
{
	UNUSED(pdrv);

	UINT * 		ptr = (UINT *)buff;
	DRESULT 	result = RES_ERROR;

	switch (cmd)
	{
		case CTRL_SYNC:
		{
			// All writing takes place inside disk_write, so this command will never be used
			result = RES_OK;
			break;
		}
		case GET_SECTOR_COUNT:
		{
			ptr[0] = SD_get_capacity() / SD_BLOCK_SIZE;
			// ptr[0] = 128;
			result = RES_OK;
			break;
		}
		case GET_SECTOR_SIZE:
		{
			// FF_MAX_SS == FF_MIN_SS, so this command will never be used
			ptr[0] = SD_BLOCK_SIZE;
			result = RES_OK;
			break;
		}
		case GET_BLOCK_SIZE:
		{
			// ptr[0] = 256;
			ptr[0] = 1;
			result = RES_OK;
			break;
		}
		case CTRL_TRIM:
		{
			// Not required
			result = RES_OK;
			break;
		}
	}

	return result;
}

int ff_mutex_create(int vol)
{
	UNUSED(vol);
	fsif_info.semaphore = ARCADIA_semaphore_alloc(&fsif_info.semaphore_buffer);
	return (int)(fsif_info.semaphore != NULL);
}

void ff_mutex_delete(int vol)
{
	UNUSED(vol);
	ARCADIA_semaphore_free(fsif_info.semaphore);
}

int ff_mutex_take(int vol)
{
	UNUSED(vol);
	return (int)(ARCADIA_semaphore_take(fsif_info.semaphore) == pdTRUE);
}

void ff_mutex_give(int vol)
{
	UNUSED(vol);
	ARCADIA_semaphore_give(fsif_info.semaphore);
}

FRESULT FSIF_f_mkfs(void)
{
	FATFS fs;           /* Filesystem object */
    FIL fil;            /* File object */
    FRESULT res;        /* API result code */
    UINT bw;            /* Bytes written */
    BYTE work[FF_MAX_SS]; /* Work area (larger is better for processing time) */


    /* Format the default drive with default parameters */
    res = f_mkfs("", 0, work, sizeof(work));

	FSIF_LOG_DBG("f_mkfs: %u\r\n", res);
	

    // /* Give a work area to the default drive */
    // f_mount(&fs, "", 0);

	// FSIF_LOG_DBG("opening\r\n");
    // /* Create a file as new */
    // res = f_open(&fil, "hello.txt", FA_CREATE_NEW | FA_WRITE);
    // if (res)
	// {
	// 	FSIF_LOG_DBG("f_open: %u\r\n", res);
	// }

	// FSIF_LOG_DBG("writing\r\n");
    // /* Write a message */
    // f_write(&fil, "Hello, World!\r\n", 15, &bw);
    // if (bw != 15)
	// {
	// 	FSIF_LOG_DBG("f_write: %u\r\n", res);		
	// }

	// FSIF_LOG_DBG("closing\r\n");
    // /* Close the file */
    // f_close(&fil);

	// FSIF_LOG_DBG("unmounting\r\n");
    // /* Unregister work area */
    // f_mount(0, "", 0);

	return res;

}

void FSIF_fs_init(void)
{
	FRESULT result;

	if (SD_card_init())
	{
		result = FSIF_f_mount();

		if (FR_OK == result)
		{
			FSIF_LOG_DBG("File system mounted\r\n");
		}
		else
		{
			FSIF_LOG_WARN("Failed to mount file system (status: %u)\r\n", result);
		}
	}
	else
	{
		FSIF_LOG_WARN("Failed to initialize SD card\r\n");
	}
}

FRESULT FSIF_f_mount(void)
{
	return f_mount(&fsif_info.fs, "", 0);
}

FRESULT FSIF_f_open(const char * kpc_fname, char * buf, uint32_t * bw, uint32_t * br)
{
    FIL file;
    FRESULT result = f_open(&file, kpc_fname, FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
    UINT bytes_written;
    UINT bytes_read;

    if (FR_OK != result)
    {
        return result;
    }

    // Write to the file
    // result = f_write(&file, "This is a test\0", strlen("This is a test\0"), &bytes_written);
    // *bw = bytes_written;

    if (FR_OK == result)
    {
        // Reset file pointer to the beginning
        result = f_lseek(&file, 0);
        if (FR_OK != result)
        {
            f_close(&file);
            return result;
        }

        // Read from the file
        result = f_read(&file, buf, 14, &bytes_read);
        *br = bytes_read;
    }

    f_close(&file);

    return result;
}

// FRESULT FSIF_f_read(void)
// {

// }

