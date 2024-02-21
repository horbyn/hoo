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
    //test_linked_list();

    int a = 10, b = 0;
    int c = a / b;
    kprintf("c = %d\n", c);

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
        "kern base = 0x%x,  kern end = 0x%x\n"
        "kern length = %dkb\n\n",
        (uint32_t)__kern_base, (uint32_t)__kern_end,
        ((uint32_t)__kern_end - (uint32_t)__kern_base) / 1024);

    init_ata();
    create_kernel_idle();
    init_fs();
}
