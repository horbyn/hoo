/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "first.h"

/**
 * @brief ring3 main process
 */
void
ring3_first(void) {
    // launch from idle ring3
    int result = sys_exec(BUILT_SHELL);
    if (result == -1)    sys_printf("cannot launch shell\n");
}
