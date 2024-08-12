/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "loader.h"
#include "kern/driver/io.h"///

/**
 * @brief the specific filename append to base filename
 * 
 * @param base_buff   base filename buffer
 * @param append_buff the result buffer
 * @param filename    the specific filename
 */
static void
filename_append(const char *base_buff, char *append_buff, const char *filename) {
    if (base_buff == null || append_buff == null || filename == null)
        panic("filename_append(): null pointer");

    memmove(append_buff, base_buff, strlen(base_buff));
    memmove(append_buff + strlen(append_buff), filename, strlen(filename));
}

/**
 * @brief convert builtin commands to file
 * 
 * @param filename file name
 * @param addr builtin commands memory address
 * @param len  builtin commands length
 */
static void
builtin_to_file(const char *filename, void *addr, uint32_t len) {
    if (filename == null || addr == null || len == 0)    return;

    char *specific_file = dyn_alloc(64);
    bzero(specific_file, 64);
    filename_append(DIR_LOADER, specific_file, filename);
    kprintf("\tbuiltin_to_file(): sp file: %s\n", specific_file);
    files_create(specific_file);
    fd_t fd = files_open(specific_file);
    kprintf("\tbuiltin_to_file(): fd: %d\n", fd);
    files_write(fd, addr, len);
    files_close(fd);

    dyn_free(specific_file);
}

/**
 * @brief load builtin commands
 */
void
load_builtins(void) {
    files_create(DIR_LOADER);
    kprintf("load_builtins(): ls base: 0x%x, len: %d\n", (uint32_t)__ls_base,
        (uint32_t)__ls_end - (uint32_t)__ls_base);
    builtin_to_file(BUILT_LS, (void *)__ls_base, (uint32_t)__ls_end - (uint32_t)__ls_base);
}
