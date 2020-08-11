ENTRY(start)
SECTIONS
{
	. = 0x10000;
	. = ALIGN(4);
	.text :
	{
		head.o(.text)
		*(.text)
	}

	. = ALIGN(4);
	.data :
	{
		*(.data)
	}

	. = ALIGN(4);
	.rodata :
	{
		*(.rodata);
	}

	. = ALIGN(4);
	.bss :
	{
		*(.bss);
	}
}
