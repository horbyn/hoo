/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "builtin_ls.h"

void
main_ls(void) {
    unsigned int val = 0;
    (void)val;
    static char arr[4];
    (void)arr;
    __asm__ ("movl $0x4, %eax");
}
