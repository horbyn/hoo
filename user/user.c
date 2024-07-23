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
    syscall_entry(SYS_PRINTF, 0);
}

/**
 * @brief create files or directories
 * 
 * @param filename file or directory name
 */
void
create(const char *filename) {
    syscall_entry(SYS_CREATE, 0);
}

/**
 * @brief delete files or directories
 * 
 * @param filename file or directory name
 */
void
remove(const char *filename) {
    syscall_entry(SYS_REMOVE, 0);
}

/**
 * @brief open the specific file
 * 
 * @param filename file name
 * @return file descriptor
 */
int
open(const char *filename) {
    int fd = -1;
    syscall_entry(SYS_OPEN, &fd);
    return fd;
}

/**
 * @brief close the specific file
 * 
 * @param fd file descriptor
 */
void
close(int fd) {
    syscall_entry(SYS_CLOSE, 0);
}

/**
 * @brief read data from the specific file
 * 
 * @param fd    file descriptor
 * @param buf   buffer to store data
 * @param count amount
 */
void
read(int fd, void *buf, unsigned int count) {
    syscall_entry(SYS_READ, 0);
}

/**
 * @brief write data to the specific file
 * 
 * @param fd    file descriptor
 * @param buf   data to write
 * @param count amount
 */
void
write(int fd, const void *buf, unsigned int count) {
    syscall_entry(SYS_WRITE, 0);
}
