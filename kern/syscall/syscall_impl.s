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
    .globl printf

# 80th interrupt handling routine
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

    addl %ebx,       %esp
    popl %ebp
    ret

# ################################# #
#                                   #
# @brief formatting output          #
#                                   #
# @param format:    format string   #
# @param ...:       more parameters #
#                                   #
# ################################# #
printf:
    pushl %ebp
    movl %esp,          %ebp
    pushal                                                  # save user context

    movl $1,            %eax
    lea (%ebp, %eax, 8),%ebx                                # save user ring3 esp
    movl (%ebp),        %ecx                                # save user ring3 ebp
    movl $0,            %eax
    int $0x80

    popal                                                   # restore user context
    popl %ebp
    ret
