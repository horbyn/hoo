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
        EXCLUDE_FILE(user/builtin_ls.o user/builtin_shell.o)
        *(.text)
    }

    . = ALIGN(4);
    .data : AT(ADDR(.data) - 0x80000000) {
        EXCLUDE_FILE(user/builtin_ls.o user/builtin_shell.o)
        *(.data)
    }
    . = ALIGN(4);
    .bss : AT(ADDR(.bss) - 0x80000000) {
        EXCLUDE_FILE(user/builtin_ls.o user/builtin_shell.o)
        *(.bss)
    }

    PROVIDE(__kern_end = . - 0x80000000);
}
