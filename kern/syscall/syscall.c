/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "syscall.h"
#include "kern/fs/files.h"
#include "user/syscall_num.h"

/**
 * @brief system call stub array
 */
syscall_t __stub[MAX_SYSCALL];

/**
 * @brief system calls initialization
 */
void syscall_init(void) {
    __stub[SYS_CREATE]      = (syscall_t)files_create;
    __stub[SYS_REMOVE]      = (syscall_t)files_remove;
    __stub[SYS_OPEN]        = (syscall_t)files_open;
    __stub[SYS_CLOSE]       = (syscall_t)files_close;
    __stub[SYS_READ]        = (syscall_t)files_read;
    __stub[SYS_WRITE]       = (syscall_t)files_write;
    __stub[MAX_SYSCALL - 1] = null;
}
