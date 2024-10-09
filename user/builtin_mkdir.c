/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "builtin_mkdir.h"

#define MAX_PATH_LEN    512

/**
 * @brief mkdir command
 * 
 * @param argc parameter number
 * @param argv parameter(s)
 */
void
main_mkdir(int argc, char **argv) {
    if (argc == 1) {
        sys_printf("mkdir: missing operand\n");
        return;
    }

    // handle the absolute path
    char *abs = alloc(MAX_PATH_LEN);
    bzero(abs, MAX_PATH_LEN);
    if (workingdir(abs, MAX_PATH_LEN) != 0) {
        sys_printf("mkdir: failed to get the current working directory\n");
        free(abs);
        return;
    }
    memmove(abs + strlen(abs), argv[1], strlen(argv[1]));

    // `sys_create()` determines whether it is a directory or a file based on
    // whether the last character of the parameter is '/'
    uint32_t len = strlen(abs);
    if (abs[len - 1] != '/') {
        abs[len] = '/';
        abs[len + 1] = 0;
    }

    sys_create(abs);
    free(abs);
}
