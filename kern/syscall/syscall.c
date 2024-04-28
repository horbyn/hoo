/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "syscall.h"

/**
 * @brief system call stub array
 */
syscall_t __stub[MAX_SYSCALL] = {
    // #0
    (syscall_t)kprintf,

    // MUST BE null terminated
    0
};
