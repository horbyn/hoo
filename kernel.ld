ENTRY(entry)
SECTIONS {
    /* VMA is the logic address;
       while LMA is the real loading address */

    . = 0x1000;
    PROVIDE(__kern_base = .);
    .text : AT(0x1000) {
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
