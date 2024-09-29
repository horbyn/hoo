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
    __stub[SYS_OPEN]   = (syscall_t)files_open;
    __stub[SYS_CLOSE]  = (syscall_t)files_close;
    __stub[SYS_READ]   = (syscall_t)files_read;
    __stub[SYS_WRITE]  = (syscall_t)files_write;
    __stub[SYS_FORK]   = (syscall_t)fork;
    __stub[SYS_EXEC]   = (syscall_t)exec;
    __stub[SYS_WAIT]   = (syscall_t)wait_child;
    __stub[SYS_EXIT]   = (syscall_t)exit;
    __stub[MAX_SYSCALL - 1] = null;
}
