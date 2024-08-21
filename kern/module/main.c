/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "main.h"

/**
 * @brief kernel ring3 main process
 */
void
main(void) {
    // hoo ring3 jump into shell process and not return
    int result = sys_exec(BUILT_SHELL);
    if (result == -1)    sys_printf("cannot launch shell\n");
}
