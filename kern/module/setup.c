/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "setup.h"

void
setup_kernel_gdt(void) {
    set_gdt(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);         // #0 null
    set_gdt(1, 0xfffff, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1);   // #1 kernel code
    set_gdt(2, 0xfffff, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1);   // #2 kernel data
    set_gdt(3, 0xfffff, 0, 0, 1, 0, 1, 1, 3, 1, 0, 1, 1);   // #3 user code
    set_gdt(4, 0xfffff, 0, 0, 1, 0, 0, 1, 3, 1, 0, 1, 1);   // #4 user data

    set_gdtr();
}
