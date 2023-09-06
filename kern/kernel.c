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
    init_scheduler();

    kprintf("kern base = %x\nkern end = %x\n\n",
        (uint32_t)__kern_base, (uint32_t)__kern_end);

}

/**
 * @brief kernel main thread -- idle
 */
void
idle(void) {

    while (1) {
        kprintf("Kernel ");
    }
}
