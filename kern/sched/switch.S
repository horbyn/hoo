    .text
    .code32
    .globl switch_to
    .globl mode_ring3

    # 像函数一样调用，`switch_to(node_t *cur_task, node_t *next_task)`
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

    movl 0x8(%ebp),  %eax       # 取出 cur 的 node
    cmpl $0,         %eax
    jz next
    movl (%eax),     %eax       # 取出 cur 的 PCB
    pushl $return               # 入栈返回地址以便前一个任务可以返回到这里
    movl %esp,       (%eax)     # 将 cur 的值赋值给 ESP
    popl %eax                   # 出栈返回地址

next:
    movl 0xc(%ebp),  %eax       # 取出 next 的 node
    movl (%eax),     %eax       # 取出 next 的 PCB
    movl 0xc(%eax),  %ecx       # 页目录表的物理地址
    movl %ecx,       %cr3
    movl (%eax),     %esp

    ret
return:
    popl %ebp
    ret

    # 不能视为一个函数，因为 iret 指令不能用来恢复 EIP
mode_ring3:
    pushl %ebp
    movl %esp,           %ebp

    movl $((4 * 8) | 3), %eax
    movl %eax,           %ds
    movl %eax,           %es
    movl %eax,           %fs
    movl %eax,           %gs

    movl 0x4(%ebp),      %eax   # 取出用户栈
    pushl $((4 * 8) | 3)        # 入栈 SS
    pushl %eax                  # 入栈 ESP
    pushf                       # 入栈和 ring0 一致的 EFLAGS
    orl $0x200,          %ss:(%esp)
    pushl $((3 * 8) | 3)
    movl 0x8(%ebp),      %eax   # 取出用户入口
    pushl %eax

    # 重置除了 ESP 外的寄存器
    xorl %eax,           %eax
    xorl %ebx,           %ebx
    xorl %ecx,           %ecx
    xorl %edx,           %edx
    xorl %esi,           %esi
    xorl %edi,           %edi
    movl 0x4(%ebp),      %ebp   # 用户栈栈底

    iret
