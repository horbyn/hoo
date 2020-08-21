SECTIONS
{
	. = ALIGN(4);
	. = 0x10000;
	.text :
	{
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
		*(.rodata)
	}

	. = ALIGN(4);
	.bss :
	{
		*(.bss)
	}
}
