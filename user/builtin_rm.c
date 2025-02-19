#include "builtin_rm.h"
#include "lib.h"

#define MAX_PATH_LEN    512

/**
 * @brief rm 命令
 * 
 * @param argc 参数个数
 * @param argv 参数
 */
void
main_rm(int argc, char **argv) {
    if (argc == 1 || (argc > 1 && argv[1] == 0)) {
        sys_printf("rm: missing operand\n");
        return;
    }

    // 处理绝对路径
    char *abs = alloc(MAX_PATH_LEN);
    bzero(abs, MAX_PATH_LEN);
    if (argv[1][0] != '/') {
        if (workingdir(abs, MAX_PATH_LEN) != 0) {
            sys_printf("rm: failed to get the current working directory\n");
            free(abs);
            return;
        }
        memmove(abs + strlen(abs), argv[1], strlen(argv[1]));
    } else {
        memmove(abs, argv[1], strlen(argv[1]));
    }

    int ret = sys_remove(abs);
    if (ret == -1)    sys_printf("rm: no parent directory\n");
    else if (ret == -2)    sys_printf("rm: invalid parent directory\n");
    else if (ret == -3)    sys_printf("rm: no such file or directory\n");
    free(abs);
}
