/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "kernel.h"

/**
 * @brief kernel run!
 */
void
kernel_exec(void) {
    kernel_init();
    enable_intr();

    //test_kernel_threads();
    //test_disk_read();

    // DONT RETURN
    while(1);
}

/**
 * @brief kernel initialization
 */
void
kernel_init(void) {

    init_pic();
    init_pit();
    init_interrupt();
    init_cga();
    init_scheduler();

    kprintf("================ KERNEL IMAGE ================\n"
        "kern base = 0x%x,  kern end = 0x%x\n\n",
        (uint32_t)__kern_base, (uint32_t)__kern_end);

    init_ata();
    create_kernel_idle();
    init_fs();
}
