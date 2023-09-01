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
    idle();

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
    enable_intr();
    kprintf("kern base = %x\nkern end = %x\n\n",
        (uint32_t)__kern_base, (uint32_t)__kern_end);

    disable_intr();
    init_scheduler();
    enable_intr();
}

/**
 * @brief kernel main thread -- idle
 */
void
idle(void) {
    while (1) {
        //wait(&__spinlock_disp);
        //kprintf("%x ", user);
        kprintf("Kernel ");
        //signal(&__spinlock_disp);
    }
}
