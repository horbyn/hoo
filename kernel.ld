ENTRY(entry)
SECTIONS {
    /* VMA is the logic address;
       while LMA is the real loading address */

    /* Now paging feature is disable;
       so we need to enable in absolute address */
    . = 0;
    .init.text : {
        kern/entry.o(.text)
        . = ALIGN(4);
        conf/Config.o(.text)
    }
    .init.data : {
        . = ALIGN(4);
        *(.config)
        /* MUST be aligned 4096 because
           we set page dir/page table here */
        . = ALIGN(4096);
        *(.page)
    }

    /* After paging
     */
    . = ALIGN(4);
    . += 0xc0000000;
    PROVIDE(__kern_base = .);
    .text : AT(ADDR(.text) - 0xc0000000) {
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
    PROVIDE(__kern_end = .);
}
