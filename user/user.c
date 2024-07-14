/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "user.h"

/**
 * @brief formatting print
 * 
 * @param format formatting string
 */
void
printf(const char *format, ...) {
    syscall_entry(SYS_PRINTF);
}

/**
 * @brief create files or directories
 * 
 * @param filename file or directory name
 */
void
create(const char *filename) {
    syscall_entry(SYS_CREATE);
}

/**
 * @brief delete files or directories
 * 
 * @param filename file or directory name
 */
void
remove(const char *filename) {
    syscall_entry(SYS_REMOVE);
}
