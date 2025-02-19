#include "builtin_pwd.h"
#include "lib.h"

/**
 * @brief pwd 命令
 * 
 * @param argc 参数个数
 * @param argv 参数
 */
void
main_pwd(int argc, char **argv) {
    char *wd = alloc(512);
    if (workingdir(wd, 512) == 0)    sys_printf("%s\n", wd);
    else    sys_printf("pwd: cannot get current directory\n");
    free(wd);
}
