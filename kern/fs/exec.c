/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "exec.h"

/**
 * @brief change the control flow to execute the specific program
 * 
 * @param filename the specific program filename
 * @retval 0: success
 * @retval -1: command not found
 */
int
exec(const char *filename) {
    if (filename == null)
        panic("exec(): null pointer");

    // formatting the filename
    static const uint32_t MAXSIZE_PATH = 512;
    char *absolute_path = dyn_alloc(MAXSIZE_PATH);
    bzero(absolute_path, MAXSIZE_PATH);
    if (filename[0] != '/') {
        uint32_t size = strlen(DIR_LOADER);
        memmove(absolute_path, DIR_LOADER, size);
        memmove(absolute_path + size, filename, strlen(filename));
    } else
        memmove(absolute_path, filename, strlen(filename));

    // data reading from the file to memory
    fd_t fd = files_open(absolute_path);
    if (fd == -1)    return -1;
    uint32_t file_size = files_get_size(fd);
    uint8_t *program = dyn_alloc(file_size);
    files_read(fd, (char *)program, file_size);

    // change the control flow
    __asm__ ("movl %0, %%eax\n\t"
        "call *%%eax" : : "r"(program));

    dyn_free(program);
    files_close(fd);
    dyn_free(absolute_path);
    return 0;
}
