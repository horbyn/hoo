SECTIONS
{
    . = 0x10;
    .text :
    {
        *(.text)
    }
    . = 0x200;
    .data :
    {
        *(.data)
    }
    . = ALIGN(8);
    .bss :
    {
        *(.bss)
    }
}