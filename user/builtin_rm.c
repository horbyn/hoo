/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "builtin_rm.h"
#include "lib.h"

#define MAX_PATH_LEN    512

/**
 * @brief rm command
 * 
 * @param argc parameter number
 * @param argv parameter(s)
 */
void
main_rm(int argc, char **argv) {
    if (argc == 1 || (argc > 1 && argv[1] == 0)) {
        sys_printf("rm: missing operand\n");
        return;
    }

    // handle the absolute path
    char *abs = alloc(MAX_PATH_LEN);
    bzero(abs, MAX_PATH_LEN);
    if (workingdir(abs, MAX_PATH_LEN) != 0) {
        sys_printf("rm: failed to get the current working directory\n");
        free(abs);
        return;
    }
    memmove(abs + strlen(abs), argv[1], strlen(argv[1]));

    sys_remove(abs);
    free(abs);
}
