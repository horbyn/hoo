/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/

    .text
    .code32
    .globl switch_to

    # called as a function, `switch_to(node_t *cur_task, node_t *next_task)`
    # ASSUME that `next_task` IS NOT NULL because it needs not to schedule without next tasks
switch_to:
    pushl %ebp
    movl %esp,      %ebp

    # #######################################
    # stack frame:                          #
    # ┌──────────────────┐                  #
    # │ param: next_task │                  #
    # ├──────────────────┤+0xc              #
    # │ param: cur_task  │                  #
    # ├──────────────────┤+8                #
    # │    return addr   │                  #
    # ├──────────────────┤+4                #
    # │     prev  ebp    │                  #
    # ├──────────────────┤<-- current ebp   #
    # │      ... ...     │                  #
    # #######################################

    movl 0x8(%ebp), %eax    # fetch `node_t` pointer of cur.
    cmpl $0,        %eax
    jz next
    pushl $return           # push the return addr in order to the prev. task could return here
    movl %esp,      (%eax)  # current `esp` assigns to the value the cur. pcb pointed to
    popl %eax               # discard the prev. return addr

next:
    movl 0xc(%ebp), %eax    # fetch `node_t` pointer of next
    movl (%eax),    %esp    # fetch the value the next pcb pointed to

    ret
return:
    popl %ebp
    ret
