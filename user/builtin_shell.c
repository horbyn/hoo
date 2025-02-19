#include "builtin_shell.h"
#include "types.h"

static char TMP_PROMPT[] = "[hoo]";
static char command[MAX_CMD_LEN];

/**
 * @brief shell 命令
 * 
 * @param argc 参数个数
 * @param argv 参数
 */
void
main_shell(int argc, char **argv) {
    char ch = 0;
    int i = 0;
    int flong = 0;

    // 目录结构开始于根目录
    sys_cd("/");

    while (1) {
        sys_printf("%s ", TMP_PROMPT);
        i = 0;

        // 读取命令
        do {
            sys_read(FD_STDIN, &ch, 1);

            if (ch != '\b')    sys_printf("%c", ch);
            else {
                if (i > 0) {
                    sys_printf("\b");
                    --i;
                }
            }

            if (32 <= ch && ch <= 126) {
                if (i < MAX_CMD_LEN)    command[i++] = ch;
                else {
                    sys_printf(" (Command beyond %d characters!)\n", MAX_CMD_LEN);
                    flong = 1;
                    break;
                }
            }
        } while (ch != '\n');
        if (flong || i == 0) {
            flong = 0;
            continue;
        }

        // 解析命令
        command[i] = 0;
        int pid = sys_fork();
        if (pid != 0) {
            // 父进程
            sys_wait();
        } else {
            // 子进程
            sys_exec(command);
        }
    } // end while()
}
