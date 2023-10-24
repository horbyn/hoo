/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
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
    test_disk_read();

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

    kprintf("kern base = %x\nkern end = %x\n\n",
        (uint32_t)__kern_base, (uint32_t)__kern_end);

    create_kernel_idle();
}
