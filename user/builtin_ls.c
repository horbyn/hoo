/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "builtin_ls.h"

/**
 * @brief ls command
 * 
 * @param argc parameter number
 * @param argv parameter(s)
 */
void
main_ls(int argc, char **argv) {
    char *param = argc > 1 ? argv[1] : 0;
    int ret = sys_ls(param);
    if (ret == -1)    sys_printf("ls: \"%s\" No such file or directory\n", param);
}