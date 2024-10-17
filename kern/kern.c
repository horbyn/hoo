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
kern_init() {
    kinit_io();
    kinit_config();
    kinit_isr_idt();
    kinit_driver();
    kinit_memory();
}

/**
 * @brief kernel run
 */
void
kern_exec(void) {
    ENABLE_INTR();
}
