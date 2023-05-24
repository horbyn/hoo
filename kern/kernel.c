/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "kernel.h"

void
kernel_exec() {
    setup_vmm();

    setup_kernel_gdt();
    setup_pmm();
}
