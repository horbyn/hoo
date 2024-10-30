/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/

    .text
    .code32
    .extern __stub
    .globl syscall

# 0x80th interrupt handling routine
syscall:
    pushl %ebp
    movl %esp, %ebp

    # #######################################
    # user context stores in stack frame:   #
    # ┌──────────────────┐                  #
    # │        eax       │                  #
    # ├──────────────────┤+0x24             #
    # │        ecx       │                  #
    # ├──────────────────┤+0x20             #
    # │        edx       │                  #
    # ├──────────────────┤+0x1c             #
    # │        ebx       │                  #
    # ├──────────────────┤+0x18             #
    # │        esp       │                  #
    # ├──────────────────┤+0x14             #
    # │        ebp       │                  #
    # ├──────────────────┤+0x10             #
    # │        esi       │                  #
    # ├──────────────────┤+0xc              #
    # │        edi       │                  #
    # ├──────────────────┤+8                #
    # │    return addr   │                  #
    # ├──────────────────┤+4                #
    # │     prev  ebp    │                  #
    # ├──────────────────┤<-- current ebp   #
    # │      ... ...     │                  #
    # #######################################

    # fetch user ring3 stack frame
    movl 0x20(%ebp), %eax                                   # eax is equal to ring3 ebp
    movl 0x18(%ebp), %ebx                                   # ebx is equal to ring3 esp

    # ss: ring3.esp --> ss: ring0.esp
    movl %eax,       %ecx
    subl %ebx,       %ecx
    addl $4,         %ecx
    movl %ecx,       %edx

    movl %ebx,       %esi
    subl %ecx,       %esp
    movl %esp,       %edi
    rep movsb

    movl %edx,       %ebx                                   # %ebx is callee-saved so used to stash the counter

    movl 0x24(%ebp), %eax                                   # eax
    call *__stub(, %eax, 4)                                 # addr = $isr + %eax * $4

    movl 0x1c(%ebp), %ecx
    cmpl $0,         %ecx
    jz syscall_exit
    movl %eax,       (%ecx)

syscall_exit:
    addl %ebx,       %esp
    popl %ebp
    ret
