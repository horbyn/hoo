#include "builtin_ls.h"

/**
 * @brief ls 命令
 * 
 * @param argc 参数个数
 * @param argv 参数
 */
void
main_ls(int argc, char **argv) {
    char *param = argc > 1 ? argv[1] : 0;
    int ret = sys_ls(param);
    if (ret == -1)    sys_printf("ls: \"%s\" No such file or directory\n", param);
}