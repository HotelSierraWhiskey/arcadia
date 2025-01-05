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

#define FSIF_LOG_DBG(fmt, ...)   		SHELL_printf("\r%-10s" fmt, "[FSIF]", ##__VA_ARGS__)
#define FSIF_LOG_WARN(fmt, ...)   		SHELL_PRINT_WARNING("\r%-10s" fmt, "[FSIF]", ##__VA_ARGS__)

#define FSIF_DEFAULT_DRIVE_PATH			""
#define FSIF_DISK_RW_RETRIES			(5)
#define FSIF_FS_LABEL_NAME_LEN_MAX		(12)


typedef enum _FSIF_fs_type_id
{
	FSIF_FS_TYPE_ID_INVALID = 0,
	FSIF_FS_TYPE_ID_FAT12,
	FSIF_FS_TYPE_ID_FAT16,
	FSIF_FS_TYPE_ID_FAT32,
	FSIF_FS_TYPE_ID_EXFAT,
	//////////
	FSIF_FS_TYPE_ID_NUM_IDS
} FSIF_fs_type_id_t;

typedef struct _FSIF_info
{
	FATFS				fs;
	char				pc_label[FSIF_FS_LABEL_NAME_LEN_MAX];
	SemaphoreHandle_t	semaphore;
	StaticSemaphore_t	semaphore_buffer;
} FSIF_info_t;

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

static const char * const kpc_fat_subtype[] =
{
	[FSIF_FS_TYPE_ID_INVALID] 	= "Invalid",
	[FSIF_FS_TYPE_ID_FAT12] 	= "FAT12",
	[FSIF_FS_TYPE_ID_FAT16] 	= "FAT16",
	[FSIF_FS_TYPE_ID_FAT32] 	= "FAT32",
	[FSIF_FS_TYPE_ID_EXFAT] 	= "EXFAT"
};

static FSIF_info_t fsif_info;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	Internal FatFs Interface Function
 *
 * 	@warning Don't call this directly
 *	
 *	Basically a stub to make FatFs happy. We perform hardware initialization independently.
 *
 * 	@param[in] pdrv The drive index (unused)
 * 
 * 	@return 0 always
 * 
 ****************************************************************************************************/
DSTATUS disk_initialize(BYTE pdrv)
{
	UNUSED(pdrv);

	return 0;
}

/****************************************************************************************************
 *	Internal FatFs Interface Function
 *
 * 	@warning Don't call this directly
 *
 * 	Returns the initialization state of the SD hardware
 *
 * 	@param[in] pdrv The drive index (unused)
 * 
 * 	@return 0 always
 * 
 ****************************************************************************************************/
DSTATUS disk_status(BYTE pdrv)
{
	UNUSED(pdrv);

	if (SD_is_initialized())
	{
		return 0;
	}

	return STA_NOINIT;
}

/****************************************************************************************************
 *	Internal FatFs Interface Function
 *
 * 	@warning Don't call this directly
 *
 * 	Wrapper for SD block reads
 *
 * 	@param[in] 	pdrv The drive index (unused)
 * 	@param[out] buff A buffer to store the data to be read
 * 	@param[in] 	sector The sector index
 * 	@param[in] 	count The number of sectors to read
 * 
 * 	@return RES_OK always
 * 
 ****************************************************************************************************/
DRESULT disk_read(BYTE pdrv, BYTE* buff, LBA_t sector, UINT count)
{
	UNUSED(pdrv);

	DRESULT d_result = RES_ERROR;
	uint8_t u8_retries = FSIF_DISK_RW_RETRIES;

	for (uint8_t i = 0; i < count; i++)
	{
		do
		{
			if (0 == SD_read_block((sector + i) * SD_BLOCK_SIZE, buff + (i * SD_BLOCK_SIZE)))
			{
				d_result = RES_OK;
				u8_retries = FSIF_DISK_RW_RETRIES;
			}
			else
			{
				d_result = RES_ERROR;
			}
		} while (d_result != RES_OK && --u8_retries);
	}

	return d_result;
}

/****************************************************************************************************
 *	Internal FatFs Interface Function
 *
 * 	@warning Don't call this directly
 *
 * 	Wrapper for SD block writes
 *
 * 	@param[in] 	pdrv The drive index (unused)
 * 	@param[in] 	buff The data to write
 * 	@param[in] 	sector The sector index
 * 	@param[in] 	count The number of sectors to write to
 * 
 * 	@return RES_OK always
 * 
 ****************************************************************************************************/
DRESULT disk_write(BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count)
{
	UNUSED(pdrv);

	DRESULT d_result = RES_ERROR;
	uint8_t u8_retries = FSIF_DISK_RW_RETRIES;

	for (uint8_t i = 0; i < count; i++)
	{
		do
		{
			if (0 == SD_write_block((sector + i) * SD_BLOCK_SIZE, buff + (i * SD_BLOCK_SIZE)))
			{
				d_result = RES_OK;
				u8_retries = FSIF_DISK_RW_RETRIES;
			}
			else
			{
				d_result = RES_ERROR;
			}
		} while (d_result != RES_OK && --u8_retries);
	}

	return d_result;
}

/****************************************************************************************************
 *	Internal FatFs Interface Function
 *
 * 	@warning Don't call this directly
 *
 * 	Command and control interface function for FatFs
 *
 * 	@param[in] 		pdrv The drive index (unused)
 * 	@param[in] 		cmd The command to issue
 * 	@param[in, out]	buff generic buffer for command params and data
 * 
 * 	@return RES_OK if all's well, else RES_ERROR
 * 
 ****************************************************************************************************/
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
			*ptr = (uint32_t)((SD_get_capacity() / SD_BLOCK_SIZE));
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
			// Erase block size in units of sector
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

/****************************************************************************************************
 *	Internal FatFs Interface Function
 *
 * 	@warning Don't call this directly
 * 
 * 	Creates a FreeRTOS mutex for FatFs to use in thread-safe mode
 *
 * 	@param[in] vol Unused
 * 
 * 	@return 1 if the mutex was created, else 0
 * 
 ****************************************************************************************************/
int ff_mutex_create(int vol)
{
	UNUSED(vol);
	fsif_info.semaphore = ARCADIA_semaphore_alloc(&fsif_info.semaphore_buffer);
	return (int)(fsif_info.semaphore != NULL);
}

/****************************************************************************************************
 *	Internal FatFs Interface Function
 *
 * 	@warning Don't call this directly
 * 
 * 	Deletes the created FreeRTOS mutex
 *
 * 	@param[in] vol Unused
 *
 ****************************************************************************************************/
void ff_mutex_delete(int vol)
{
	UNUSED(vol);
	ARCADIA_semaphore_free(fsif_info.semaphore);
}

/****************************************************************************************************
 *	Internal FatFs Interface Function
 *
 * 	@warning Don't call this directly
 * 
 * 	Obtains the created FreeRTOS mutex
 *
 * 	@param[in] vol Unused
 *
 ****************************************************************************************************/
int ff_mutex_take(int vol)
{
	UNUSED(vol);
	return (int)(ARCADIA_semaphore_take(fsif_info.semaphore) == pdTRUE);
}

/****************************************************************************************************
 *	Internal FatFs Interface Function
 *
 * 	@warning Don't call this directly
 * 
 * 	Released the created FreeRTOS mutex
 *
 * 	@param[in] vol Unused
 *
 ****************************************************************************************************/
void ff_mutex_give(int vol)
{
	UNUSED(vol);
	ARCADIA_semaphore_give(fsif_info.semaphore);
}

/****************************************************************************************************
 *	File system initialization sequence
 *
 * 	Attempts to initialize the SD card before mounting the FS
 *
 * 	@reference:
 * 	http://elm-chan.org/fsw/ff/doc/mount.html
 *
 *	@return `FR_OK` if everything went well. See `f_mount` for all possible return values
 ****************************************************************************************************/
bool FSIF_fs_init(void)
{
	FRESULT 	f_result;
	bool		b_result = false;

	if (SD_card_init())
	{
		f_result = FSIF_f_mount();

		if (FR_OK == f_result)
		{
			b_result = true;
		}
		else
		{
			FSIF_LOG_WARN("Failed to mount file system (status: %u)\r\n", f_result);
		}
	}
	else
	{
		FSIF_LOG_WARN("Failed to initialize SD card\r\n");
	}

	return b_result;
}

/****************************************************************************************************
 *	Formats the drive
 *
 * 	@reference:
 * 	http://elm-chan.org/fsw/ff/doc/mkfs.html
 *
 *	@return `FR_OK` if everything went well. See `f_mkfs` for all possible return values
 ****************************************************************************************************/
FRESULT FSIF_f_mkfs(void)
{
    BYTE 		workspace[FF_MAX_SS];

    const MKFS_PARM fmt_opt = {
        .fmt      = FM_FAT32, // | FM_SFD,  // FAT32 superfloppy
        .n_fat    = 2,                 	// Two FAT copies
        .align    = 512,               	// Align to 512-byte sectors
        .n_root   = 0,                 	// Ignored for FAT32
        .au_size  = 32 * 1024          	// 32 KB cluster size
    };

	return f_mkfs(FSIF_DEFAULT_DRIVE_PATH, &fmt_opt, workspace, FF_MAX_SS);
}

/****************************************************************************************************
 *	Mounts the FS
 *
 * 	@reference:
 * 	http://elm-chan.org/fsw/ff/doc/mount.html
 *
 *	@return `FR_OK` if everything went well. See `f_mount` for all possible return values
 ****************************************************************************************************/
FRESULT FSIF_f_mount(void)
{
	FRESULT f_result = f_mount(&fsif_info.fs, FSIF_DEFAULT_DRIVE_PATH, 1);

	if (FR_OK == f_result)
	{
		f_result = f_setlabel("ARCADIA");
	}

	return f_result;
}

FRESULT FSIF_f_open(const char * kpc_fname, char * buf, uint32_t * bw, uint32_t * br)
{
    FIL 		file;
    FRESULT 	f_result = f_open(&file, kpc_fname, FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
    UINT 		bytes_written;
    UINT 		bytes_read;

    if (FR_OK != f_result)
    {
        return f_result;
    }

	char * pc = "We're all gonna make it";

    // Write to the file
    f_result = f_write(&file, pc, strlen(pc), &bytes_written);
    *bw = bytes_written;

    if (FR_OK == f_result)
    {
        f_result = f_lseek(&file, 0);
        if (FR_OK != f_result)
        {
            f_close(&file);
            return f_result;
        }

        // Read from the file
        f_result = f_read(&file, buf, strlen(pc), &bytes_read);
        *br = bytes_read;
    }

    f_close(&file);

    return f_result;
}

void FSIF_f_ls(void)
{
    FRESULT		fr;
    DIR			dj;
    FILINFO 	fno;

    fr = f_findfirst(&dj, &fno, FSIF_DEFAULT_DRIVE_PATH, "*.*");

    while (fr == FR_OK && fno.fname[0])
	{
        SHELL_printf("%-20s %u\r\n", fno.fname, fno.fsize);
        fr = f_findnext(&dj, &fno);
    }

    f_closedir(&dj);
}

FATFS * FSIF_f_get_fs(void)
{
	return &fsif_info.fs;
}

const char * FSIF_get_fat_subtype(void)
{
	ASSERT(fsif_info.fs.fs_type < FSIF_FS_TYPE_ID_NUM_IDS);
	return kpc_fat_subtype[fsif_info.fs.fs_type];
}

const char * FSIF_get_volume_label(void)
{
    f_getlabel(FSIF_DEFAULT_DRIVE_PATH, fsif_info.pc_label, 0);

	return fsif_info.pc_label;
}
