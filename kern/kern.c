/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "kern.h"

/**
 * @brief kernel initialization
 */
void
kernel_init(void) {
    io_init();

#ifndef DEBUG
    kprintf("================ KERNEL IMAGE ================\n"
        "kern base: 0x%x,  kern end: 0x%x\n"
        "kern length: %dkb\n\n",
        (uint32_t)__kern_base, (uint32_t)__kern_end,
        ((uint32_t)__kern_end - (uint32_t)__kern_base) / 1024);
#endif

    kinit_memory();
    kinit_config();
    kinit_tasks_system();
    kinit_isr_idt();
    kinit_dirver();

}

/**
 * @brief kernel run
 */
void
kernel_exec(void) {
    idle_init();

    // "hoo" sleeps until "idle" completes its initialization
    sleeplock_t *hoo_slplock = get_hoo_sleeplock();
    wait(&hoo_slplock->guard_);
    sleep(hoo_slplock);
    signal(&hoo_slplock->guard_);
    enable_intr();

#ifdef TEST
    test_phypg_alloc();
    test_vspace();
    test_schedule();
#endif
}
