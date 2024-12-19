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
 * @brief formatting print
 * 
 * @param format formatting string
 */
void
sys_printf(const char *format, ...) {
    syscall_entry(SYS_PRINTF, 0);
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

/**
 * @brief change the current directory
 * 
 * @param dir the directory to change
 * 
 * @retval 0: change succeed
 * @retval -1: change failed, no such directory
 * @retval -2: change failed, the given path is a file
 * @retval -3: change failed, the directory tree is too long
 */
int
sys_cd(const char *dir) {
    int ret = -1;
    syscall_entry(SYS_CD, &ret);
    return ret;
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
 * @brief list all the files of current directory (if it is directory)
 * or the absolute name of the file
 * 
 * @param dir_or_file specify a directory or a file
 * 
 * @retval 0: normal
 * @retval -1: no such directory or file
 */
int
sys_ls(const char *dir_or_file) {
    int ret = -1;
    syscall_entry(SYS_LIST, &ret);
    return ret;
}

/**
 * @brief memory dynamic allocation
 * 
 * @param size the size to allocate
 * @return pointer pointed to the memory
 */
void *
sys_alloc(unsigned int size) {
    void *ret = 0;
    syscall_entry(SYS_ALLOC, &ret);
    return ret;
}

/**
 * @brief release memory
 * 
 * @param ptr specify the memory
 */
void
sys_free(void *ptr) {
    syscall_entry(SYS_FREE, 0);
}

/**
 * @brief get working directory
 * 
 * @param wd  buffer to store the working directory
 * @param len buffer length
 * 
 * @retval 0: succeed
 * @retval -1: failed, and the buffer will be fill in zero
 */
int
sys_workingdir(char *wd, unsigned int len) {
    int ret = 0;
    syscall_entry(SYS_WORKINGDIR, &ret);
    return ret;
}
