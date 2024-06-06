/*
	memory_map.ld

	Defines the general memory layout for both the bootloader
	and the application.
 */

MEMORY
{
	bootrom  (rx)  : ORIGIN = 0x00000000, LENGTH = 0x00008000
	approm   (rx)  : ORIGIN = 0x00000000, LENGTH = 0x00040000
	ram      (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00004000
}

_bootrom_start = ORIGIN(bootrom);
_bootrom_size  = LENGTH(bootrom);
_approm_start  = ORIGIN(approm);
_approm_size   = LENGTH(approm);
