ENTRY(entry)
SECTIONS
{
    . = 0x50;
    .text :
    {
        entry.o(.text)
        . = ALIGN(4);
        *(.text)
    }

    . = ALIGN(4);
    .data :
    {
        *(.data)
    }
    . = ALIGN(4);
    .bss :
    {
        *(.bss)
    }
}