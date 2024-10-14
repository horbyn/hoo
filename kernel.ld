ENTRY(entry)
SECTIONS {
    /* VMA is the logic address;
       while LMA is the real loading address */

    /* After paging
     */
    . = 0x80000000;
    PROVIDE(__kern_base = . - 0x80000000);
    .text : AT(ADDR(.text) - 0x80000000) {
        kern/entry.o(.text)
        *(.text)
    }

    . = ALIGN(4);
    .data : AT(ADDR(.data) - 0x80000000) {
        *(.data)
    }
    . = ALIGN(4);
    .bss : AT(ADDR(.bss) - 0x80000000) {
        *(.bss)
    }

    PROVIDE(null = . - 0x80000000);

    PROVIDE(__kern_end = . - 0x80000000);
}
