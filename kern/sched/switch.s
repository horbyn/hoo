/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/

    .text
    .code32
    .globl switch_to
    .globl mode_ring3

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
    movl 0x10(%eax), %ebx       # the physical address of page directory table
    movl %ebx,       %cr3
    movl (%eax),     %esp       # fetch the value the next pcb offset 0 pointed to

    ret
return:
    popl %ebp
    ret

    # called as a function, `mode_ring3(uint32_t *user_stack, void *user_entry)`
mode_ring3:
    pushl %ebp
    movl %esp,           %ebp

    movl $((4 * 8) | 3), %eax
    movl %eax,           %ds
    movl %eax,           %es
    movl %eax,           %fs
    movl %eax,           %gs

    movl 0x4(%ebp),      %eax   # fetch user stack
    pushl $((4 * 8) | 3)        # push ss
    pushl %eax                  # push user stack(esp)
    pushf                       # push eflags as same as ring0's
    pushl $((3 * 8) | 3)
    movl 0x8(%ebp),      %eax   # fetch user entry
    pushl %eax
    iret
