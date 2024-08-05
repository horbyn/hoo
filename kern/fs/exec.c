/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "exec.h"

/**
 * @brief change the control flow to jump to the specific entry point
 * 
 * @param prog_addr the specific program address to execute
 */
void
exec(void (*prog_addr)(void)) {
    prog_addr();
}
