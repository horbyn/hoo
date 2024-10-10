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
    kinit_memory();
    kinit_config();
    kinit_tasks_system();
    kinit_isr_idt();
    kinit_driver();
    kinit_fs();
}

/**
 * @brief kernel run
 */
void
kernel_exec(void) {
    enable_intr();
    load_builtins();
    disable_intr();

    clear_screen();
    idle_init(ring3_first);
    enable_intr();
}

/**
 * @brief check whether there are expired tasks to clear
 */
void
kernel_loop(void) {
    while (1)    kill();
}
