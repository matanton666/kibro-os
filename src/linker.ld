ENTRY(_start)
 
/* Tell where sections of the object files will be*/
SECTIONS
{
	/* one mb shift for convention */
	. = 1M;

	_KernelStart = .;
    /* put everything in 4k blocks for paging */
	.text BLOCK(4K) : ALIGN(4K)
	{
		KEEP(*(.multiboot))
		*(.text)
	}
 
	/* Read-only data. */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	.eh_frame BLOCK(4K) : ALIGN(4K)
	{
		KEEP(*(.eh_frame))
	}
 
	/* Read-write data (initialized) */
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}
 
	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}

	/* compiler components */
	.comment BLOCK(4k) : ALIGN(4k)
	{
		*(.comment)
	}

	_KernelEnd = .;
}