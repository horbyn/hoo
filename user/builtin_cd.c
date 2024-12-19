/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "builtin_cd.h"

/**
 * @brief cd command
 * 
 * @param argc parameter number
 * @param argv parameter(s)
 */
void
main_cd(int argc, char **argv) {
    char *param = argc > 1 ? argv[1] : 0;
    int ret = sys_cd(param);
    if (ret == -1)    sys_printf("cd: \"%s\" No such file or directory\n", param);
    else if (ret == -2)
        sys_printf("cd: \"%s\" The given path is a file\n", param);
}
