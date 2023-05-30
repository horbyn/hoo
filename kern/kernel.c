/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "kernel.h"

extern uint8_t __kern_base[], __kern_end[];

/**
 * @brief kernel run!
 */
void
kernel_exec(void) {
    clear_screen();
    set_cursor(0, 0);

    kprintf("kern base = %x\nkern end = %x\n\n",
        (uint32_t)__kern_base, (uint32_t)__kern_end);

    init_pic();
    init_interrupt();
}
