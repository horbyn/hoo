/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "builtin_pwd.h"

/**
 * @brief pwd command
 * 
 * @param argc parameter number
 * @param argv parameter(s)
 */
void
main_pwd(int argc, char **argv) {
    char *wd = alloc(512);
    if (workingdir(wd, 512) == 0)    sys_printf("%s\n", wd);
    else    sys_printf("pwd: cannot get current directory\n");
    free(wd);
}
