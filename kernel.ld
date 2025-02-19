ENTRY(entry)
SECTIONS {
    /* VMA 是逻辑地址，LMA 是加载到内存上的地址 */

    /* 启用分页之后 */
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

    PROVIDE(__kern_end = . - 0x80000000);
}
