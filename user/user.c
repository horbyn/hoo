/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "user.h"

/**
 * @brief syscall entry
 * 
 * @param syscall_number: syscall number
 * @param retval: return value
 */
static void
syscall_entry(int syscall_number, void *retval) {
    __asm__ ("pushal\n\t"
        "movl (%%ebp),        %%ebx\n\t"
        "addl $0x8,           %%ebx\n\t"
        "movl (%%ebp),        %%ecx\n\t"
        "movl (%%ecx),        %%ecx\n\t"
        "int $0x80\n\t"
        "popal\n\t"
        "popl %%ebp\n\t"
        "ret" : : "d"(retval), "a"(syscall_number));
}

/**
 * @brief formatting print
 * 
 * @param format formatting string
 */
void
sys_printf(const char *format, ...) {
    syscall_entry(SYS_PRINTF, 0);
}

/**
 * @brief create files or directories
 * 
 * @param filename file or directory name
 */
void
sys_create(const char *filename) {
    syscall_entry(SYS_CREATE, 0);
}

/**
 * @brief delete files or directories
 * 
 * @param filename file or directory name
 */
void
sys_remove(const char *filename) {
    syscall_entry(SYS_REMOVE, 0);
}

/**
 * @brief open the specific file
 * 
 * @param filename file name
 * @return file descriptor
 */
int
sys_open(const char *filename) {
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
sys_close(int fd) {
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
sys_read(int fd, void *buf, unsigned int count) {
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
sys_write(int fd, const void *buf, unsigned int count) {
    syscall_entry(SYS_WRITE, 0);
}

/**
 * @brief fork children process
 * 
 * @return child thread id
 */
int
sys_fork(void) {
    int tid = -1;
    unsigned int bak_entry = 0;
    __asm__ ("movl 0x8(%%ebp), %0\n\t"
        "movl 0x4(%%ebp), %%eax\n\t"
        "movl %%eax, 0x8(%%ebp)"
        : "=c"(bak_entry));
    syscall_entry(SYS_FORK, &tid);
    __asm__ ("movl %0, 0x8(%%ebp)"
        : : "a"(bak_entry));
    return tid;
}

/**
 * @brief change the control flow
 * 
 * @param program the program to execute
 */
void
sys_exec(const char *program) {
    syscall_entry(SYS_EXEC, 0);
}

/**
 * @brief parent wait for temination of child
 */
void
sys_wait() {
    unsigned int bak_sleeplock = 0;
    // the real sleeplock is also 8 bytes
    unsigned int temp_sleeplock[2];
    temp_sleeplock[0] = 0;
    temp_sleeplock[1] = 0;
    __asm__ ("movl 0x8(%%ebp), %0\n\t"
        "movl %1, 0x8(%%ebp)"
        : "=a"(bak_sleeplock)
        : "c"(temp_sleeplock));
    syscall_entry(SYS_WAIT, 0);
    __asm__ ("movl %0, 0x8(%%ebp)"
        : : "a"(bak_sleeplock));
}

/**
 * @brief process exits
 */
void
sys_exit() {
    syscall_entry(SYS_EXIT, 0);
}
