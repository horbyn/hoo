#include "builtin_mkdir.h"
#include "lib.h"

#define MAX_PATH_LEN    512

/**
 * @brief mkdir 命令
 * 
 * @param argc 参数个数
 * @param argv 参数
 */
void
main_mkdir(int argc, char **argv) {
    if (argc == 1) {
        sys_printf("mkdir: missing operand\n");
        return;
    }

    // 处理绝对路径
    char *abs = alloc(MAX_PATH_LEN);
    bzero(abs, MAX_PATH_LEN);
    if (argv[1][0] != '/') {
        if (workingdir(abs, MAX_PATH_LEN) != 0) {
            sys_printf("mkdir: failed to get the current working directory\n");
            free(abs);
            return;
        }
        memmove(abs + strlen(abs), argv[1], strlen(argv[1]));
    } else {
        memmove(abs, argv[1], strlen(argv[1]));
    }

    // `sys_create()` 通过参数的最后一个字符是否为 '/' 来判断是目录还是文件
    uint32_t len = strlen(abs);
    if (abs[len - 1] != '/') {
        abs[len] = '/';
        abs[len + 1] = 0;
    }

    int ret = sys_create(abs);
    if (ret == -1)    sys_printf("mkdir: directory was existed\n");
    free(abs);
}
