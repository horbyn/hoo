ENTRY(entry)
SECTIONS {
    /* VMA is the logic address;
       while LMA is the real loading address */

    /* here sets the VMA; must be equal of `VMA_KERN` */
    . = 0xc0010000;
    PROVIDE(__kern_base = .);
    .text : AT(0x10000) {   /* here is the LMA; must be equal of `ADDR_SEG_KERN` */
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