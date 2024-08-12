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
        EXCLUDE_FILE(user/builtin_ls.o)
        *(.text)
    }

    . = ALIGN(4);
    .data : AT(ADDR(.data) - 0x80000000) {
        EXCLUDE_FILE(user/builtin_ls.o)
        *(.data)
    }
    . = ALIGN(4);
    .bss : AT(ADDR(.bss) - 0x80000000) {
        EXCLUDE_FILE(user/builtin_ls.o)
        *(.bss)
    }
    . = ALIGN(4);
    PROVIDE(__ls_base = .);
    .ls : AT(ADDR(.ls) - 0x80000000) {
        user/builtin_ls.o(.*)
    }
    PROVIDE(__ls_end = .);
    PROVIDE(__kern_end = . - 0x80000000);
}
