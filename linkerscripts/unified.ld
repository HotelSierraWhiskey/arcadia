/* unified.ld */

MEMORY
{
	bootrom  (rx)  : ORIGIN = 0x00000000, LENGTH = 0x00004000
	approm   (rx)  : ORIGIN = 0x00004000, LENGTH = 0x00040000
	ram      (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00004000
}

INCLUDE linkerscripts/bootloader.ld
INCLUDE linkerscripts/app.ld

SECTIONS
{
    /* Bootloader section */
    .text.bootloader :
    {
        /* PROVIDE(u32_begin_bootloader_text = .); */
        /* KEEP(*(.nvic_table)) */
        app/build/app.o (.text)
        *(.text.bootloader .text .gnu.linkonce.t.*)
        *(.rodata.bootloader .rodata .gnu.linkonce.r.*)
        /* PROVIDE(u32_end_bootloader_text = .); */
    } > bootrom

    /* Application section */
    .text.app :
    {
        bootloader/build/bootloader.o (.text)
        /* PROVIDE(u32_begin_app_text = .); */
        *(.text.app .text .gnu.linkonce.t.*)
        *(.rodata.app .rodata .gnu.linkonce.r.*)
        /* PROVIDE(u32_end_app_text = .); */
    } > approm
}