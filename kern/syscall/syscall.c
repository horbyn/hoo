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

void syscall_init(void) {
    __stub[SYS_PRINTF] = (syscall_t)kprintf;
    __stub[SYS_CREATE] = (syscall_t)files_create;
    __stub[SYS_REMOVE] = (syscall_t)files_remove;
    __stub[MAX_SYSCALL - 1] = null;
}
