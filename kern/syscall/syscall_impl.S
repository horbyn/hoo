    .text
    .code32
    .extern __stub
    .globl syscall

# 系统调用 ISR，0x80 是中断向量号
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

    # 取出用户 ring3 栈帧
    movl 0x20(%ebp), %eax       # eax 是 ring3 栈底
    movl 0x18(%ebp), %ebx       # ebx 是 ring3 栈顶

    # ss: ring3.esp --> ss: ring0.esp
    movl %eax,       %ecx
    subl %ebx,       %ecx
    addl $4,         %ecx
    movl %ecx,       %edx

    movl %ebx,       %esi
    subl %ecx,       %esp
    movl %esp,       %edi
    rep movsb

    movl %edx,       %ebx       # %ebx 属于 callee-saved 所以需要缓存

    movl 0x24(%ebp), %eax
    call *__stub(, %eax, 4)     # 系统调用地址 = $isr + %eax * $4

    movl 0x1c(%ebp), %ecx
    cmpl $0,         %ecx
    jz syscall_exit
    movl %eax,       (%ecx)

syscall_exit:
    addl %ebx,       %esp
    popl %ebp
    ret
