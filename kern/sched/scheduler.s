# ###################################
#       Copyright (C)    horbyn, 2023
#             (hoRbyn4zZ@outlook.com)
# ###################################

    .text
    .code32
    .globl scheduler

    # called as a function, `scheduler(node_t *cur_task, node_t *next_task)`
scheduler:
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
    movl (%eax),    %eax    # fetch `pcb_t` pointer of cur. node
    pushl $return           # push the return addr in order to the prev. task could return here
    movl %esp,      (%eax)  # current `esp` assigns to the value the cur. pcb pointed to
    popl %eax               # discard the prev. return addr

    movl 0xc(%ebp), %eax    # fetch `node_t` pointer of next
    movl (%eax),    %eax    # fetch `pcb_t` pointer of next node
    movl (%eax),    %esp    # fecht the value the next pcb pointed to

    ret
return:
    popl %ebp
    ret
