/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "builtin_shell.h"
#include "types.h"

static char TMP_PROMPT[] = "[root]";
static char command[MAX_CMD_LEN];

/**
 * @brief shell command
 * 
 * @param argc parameter number
 * @param argv parameter(s)
 */
void
main_shell(int argc, char **argv) {
    char ch = 0;
    int i = 0;
    int flong = 0;

    // the story begins at the root directory
    sys_cd("/");

    while (1) {
        sys_printf("%s ", TMP_PROMPT);
        i = 0;

        // read commands
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

        // parse commands
        command[i] = 0;
        int pid = sys_fork();
        if (pid != 0) {
            // parent
            sys_wait();
        } else {
            // child
            sys_exec(command);
        }
    } // end while()
}
