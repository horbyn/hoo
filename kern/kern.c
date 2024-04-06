/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "kern.h"

void
kernel_init(void) {
    io_init();

    kprintf("================ KERNEL IMAGE ================\n"
        "kern base: 0x%x,  kern end: 0x%x\n"
        "kern length: %dkb\n\n",
        (uint32_t)__kern_base, (uint32_t)__kern_end,
        ((uint32_t)__kern_end - (uint32_t)__kern_base) / 1024);

    kinit_memory();
    kinit_config();
    kinit_tasks_system();
    kinit_idle_thread();
    kinit_isr_idt();
    kinit_dirver();
    enable_intr();
#ifdef TEST
    test_phypg_alloc();
    test_vspace();
#endif
}
