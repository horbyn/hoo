/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "builtin_shell.h"

static char TMP_PROMPT[] = "[root]";
static char command[CMD_MAX_LEN];

/**
 * @brief shell process
 */
void
main_shell(void) {
    char ch = 0;
    int i = 0;

    while (1) {
        sys_printf("%s ", TMP_PROMPT);
        i = 0;

        do {
            sys_read(FD_STDIN, &ch, 1);
            sys_printf("%c", ch);
            if (i < CMD_MAX_LEN)    command[i++] = ch;
        } while (ch != '\n');
    } // end while()
}
