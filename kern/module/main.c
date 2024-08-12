/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "main.h"

void
main(void) {
    tid_t tid = sys_fork();
    if (tid == 0) {
        exec(main_shell);
    } else {
        while (1);
    }
}
