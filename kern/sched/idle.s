/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/

    .text
    .code32
    .globl idle
    .extern idle_setup_vspace

idle:
    # for caller-saved
    pushl %eax
    pushl %ecx
    pushl %edx
    pushl %esi
    pushl %edi
    call idle_setup_vspace
    popl %edi
    popl %esi
    popl %edx
    popl %ecx
    popl %eax

    jmp .
