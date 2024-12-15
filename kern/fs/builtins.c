/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "builtins.h"
#include "exec.h"
#include "files.h"
#include "kern/panic.h"
#include "user/lib.h"
#include "kern/dyn/dynamic.h"

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
    if (files_create(specific_file) == 0) {
        fd_t fd = files_open(specific_file);
        if (fd == -1)    return;
        files_write(fd, addr, len);
        files_close(fd);
    }

    dyn_free(specific_file);
}

/**
 * @brief load builtin commands
 */
void
load_builtins(void) {
    files_create(DIR_LOADER);

#ifdef __BASE_BUILTIN_SH
    builtin_to_file(BUILT_SHELL, (void *)__BASE_BUILTIN_SH,
        (uint32_t)__END_BUILTIN_SH - (uint32_t)__BASE_BUILTIN_SH);
#endif
}
