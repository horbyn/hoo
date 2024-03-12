ENTRY(entry)
SECTIONS {
    /* VMA is the logic address;
       while LMA is the real loading address */

    /* After paging
     */
    . = 0xc0000000;
    PROVIDE(__kern_base = . - 0xc0000000);
    .text : AT(ADDR(.text) - 0xc0000000) {
        kern/entry.o(.text)
        *(.text)
    }

    . = ALIGN(4);
    .data : AT(ADDR(.data) - 0xc0000000) {
        *(.data)
    }
    . = ALIGN(4);
    .bss : AT(ADDR(.bss) - 0xc0000000) {
        *(.bss)
    }
    PROVIDE(__kern_end = . - 0xc0000000);
}
