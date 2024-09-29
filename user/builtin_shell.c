/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "builtin_shell.h"

static char TMP_PROMPT[] = "[root]";
static char command[CMD_MAX_LEN];

#define IS_COMMAND_CHAR(c) \
    (((c) >= '0' && (c) <= '9') || \
    ((c) >= 'a' && (c) <= 'z') || \
    ((c) >= 'A' && (c) <= 'Z'))

/**
 * @brief shell process
 */
void
main_shell(void) {
    char ch = 0;
    int i = 0;
    int flong = 0;

    while (1) {
        sys_printf("%s ", TMP_PROMPT);
        i = 0;

        do {
            sys_read(FD_STDIN, &ch, 1);

            if (ch != '\b') {
                sys_printf("%c", ch);
            } else {
                if (ch == '\b') {
                    if (i > 0) {
                        sys_printf("\b");
                        --i;
                    }
                }
            }

            if (IS_COMMAND_CHAR(ch)) {
                if (i < CMD_MAX_LEN)    command[i++] = ch;
                else {
                    sys_printf(" (Command beyond 16 characters!)\n");
                    flong = 1;
                    break;
                }
            }
        } while (ch != '\n');
        if (flong || i == 0) {
            flong = 0;
            continue;
        }

        command[i] = '\0';
        int pid = sys_fork();
        if (pid != 0) {
            sys_wait();
        } else {
            int result = sys_exec(command);
            if (result == -1)
                sys_printf("%s command not found\n", command);
            sys_exit();
        }
    } // end while()
}
