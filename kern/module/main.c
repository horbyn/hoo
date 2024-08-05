/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "main.h"

void
main(void) {
    sys_printf("[IDLE] Hello, this is idle\n");

    tid_t tid = sys_fork();
    if (tid == 0) {
        sys_printf("[IDLE CHILD] Hello, this is idle child\n");
        exec(main_shell);
    }
}
