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

    movl 0x8(%ebp),  %eax       # fetch `node_t` pointer of cur.
    cmpl $0,         %eax
    jz next
    movl (%eax),     %eax       # fetch `pcb_t` pointer of cur. node
    pushl $return               # push the return addr in order to the prev. task could return here
    movl %esp,       (%eax)     # current `esp` assigns to the value the cur. pcb offset 0 pointed to
    popl %eax                   # discard the prev. return addr

next:
    movl 0xc(%ebp),  %eax       # fetch `node_t` pointer of next
    movl (%eax),     %eax       # fetch `pcb_t` pointer of next node
    movl 0xc(%eax),  %ecx       # the physical address of page directory table
    movl %ecx,       %cr3
    movl (%eax),     %esp       # fetch the value the next pcb offset 0 pointed to

    ret
return:
    popl %ebp
    ret
