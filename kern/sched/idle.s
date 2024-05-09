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
    .extern wakeup
    .extern wait
    .extern signal
    .extern get_hoo_sleeplock

idle:
    # for caller-saved
    pushl %eax
    pushl %ecx
    pushl %edx
    pushl %esi
    pushl %edi

    # wakeup the hoo
    call idle_setup_vspace
    call get_hoo_sleeplock
    pushl %eax
    call wait
    call wakeup
    call signal
    popl %eax

    popl %edi
    popl %esi
    popl %edx
    popl %ecx
    popl %eax

    jmp .
