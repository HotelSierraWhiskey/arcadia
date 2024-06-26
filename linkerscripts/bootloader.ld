INCLUDE ./linkerscripts/memory_map.ld

/*
	bootloader.ld

	Section definitions:
	
	.text 		- machine instructions.
	.data 		- initialized data defined in the program.
	.bss 		- un-initialized global and static variables (to be initialized to 0 before starting main).
	.stack		- just contains the pointer to the stack end at the right place.
*/

SECTIONS
{
	.text :
	{
		PROVIDE(u32_begin_text = .);
		KEEP(*(.nvic_table))
		*(.text.* .text .gnu.linkonce.t.*)
		*(.rodata.* .rodata .gnu.linkonce.r.*)
    } >bootrom

    .ARM.extab : {
        *(.ARM.extab*)
    } >bootrom

    .ARM.exidx : {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } >bootrom

    PROVIDE(u32_end_text = .);
	

	.stack :
	{
		/* Reserve 4k for the stack (which grows down so is at the start of RAM) */
		. += 0x00001000;
		PROVIDE(u32_stack_top = .);
	} >ram

	.data :
	{
		PROVIDE(u32_begin_data = .);
		*(.data.* .data)
		*(vector_table)
		PROVIDE(u32_end_data = .);
	} >ram AT >bootrom

	.bss :
	{
		PROVIDE(u32_begin_bss = .);
		*(.bss.* .bss)
		*(COMMON)
		PROVIDE(u32_end_bss = .);
        PROVIDE(end = .);
	} >ram

	.note.gnu.build-id :
	{
		PROVIDE(buildID = .);
		KEEP(*(.note.gnu.build-id))
	} >bootrom

	/* Stabs debugging sections. */
	.stab 0 : { *(.stab) }
	.stabstr 0 : { *(.stabstr) }
	.stab.excl 0 : { *(.stab.excl) }
	.stab.exclstr 0 : { *(.stab.exclstr) }
	.stab.index 0 : { *(.stab.index) }
	.stab.indexstr 0 : { *(.stab.indexstr) }
	.comment 0 : { *(.comment) }

	/*
	 * DWARF debug sections.
	 * Symbols in the DWARF debugging sections are relative to the beginning
	 * of the section so we begin them at 0.
	 */
	/* DWARF 1 */
	.debug 0 : { *(.debug) }
	.line 0 : { *(.line) }
	/* GNU DWARF 1 extensions */
	.debug_srcinfo 0 : { *(.debug_srcinfo) }
	.debug_sfnames 0 : { *(.debug_sfnames) }
	/* DWARF 1.1 and DWARF 2 */
	.debug_aranges 0 : { *(.debug_aranges) }
	.debug_pubnames 0 : { *(.debug_pubnames) }
	/* DWARF 2 */
	.debug_info 0 : { *(.debug_info .gnu.linkonce.wi.*) }
	.debug_abbrev 0 : { *(.debug_abbrev) }
	.debug_line 0 : { *(.debug_line .debug_line.* .debug_line_end ) }
	.debug_frame 0 : { *(.debug_frame) }
	.debug_str 0 : { *(.debug_str) }
	.debug_loc 0 : { *(.debug_loc) }
	.debug_macinfo 0 : { *(.debug_macinfo) }
	/* SGI/MIPS DWARF 2 extensions */
	.debug_weaknames 0 : { *(.debug_weaknames) }
	.debug_funcnames 0 : { *(.debug_funcnames) }
	.debug_typenames 0 : { *(.debug_typenames) }
	.debug_varnames 0 : { *(.debug_varnames) }
	/* DWARF 3 */
	.debug_pubtypes 0 : { *(.debug_pubtypes) }
	.debug_ranges 0 : { *(.debug_ranges) }
	/* DWARF Extension. */
	.debug_macro 0 : { *(.debug_macro) }
	.debug_addr 0 : { *(.debug_addr) }
	.gnu.attributes 0 : { KEEP (*(.gnu.attributes)) }
}