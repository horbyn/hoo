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
syscall_t __stub[MAX_SYSCALL];

/**
 * @brief system calls initialization
 */
void syscall_init(void) {
    __stub[MAX_SYSCALL - 1] = null;
}
