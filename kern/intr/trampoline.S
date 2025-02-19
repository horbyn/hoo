    .text
    .code32
    .globl isr_part2
    .globl isr_part3
    .extern __isr

# trampoline 入口
isr_part2:
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs
    pushal

    movl $(2 * 8), %eax
    movl %eax,     %ds
    movl %eax,     %es

    movl 48(%esp), %eax
    call *__isr(, %eax, 4)   # ISR 地址 = $isr + %eax * $4

# trampoline 出口
isr_part3:
    popal
    popl %gs
    popl %fs
    popl %es
    popl %ds
    addl $8, %esp

    iret
