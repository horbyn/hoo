ENTRY(entry)
SECTIONS {
    . = 0x10000;
    PROVIDE(__kern_base = .);
    .text : {
        kern/entry.o(.text)
        . = ALIGN(4);
        *(.text)
    }

    . = ALIGN(4);
    .data : {
        *(.data)
    }
    . = ALIGN(4);
    .bss : {
        *(.bss)
    }
    PROVIDE(__kern_end = .);
}