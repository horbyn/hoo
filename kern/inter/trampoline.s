# ###################################
#       Copyright (C)    horbyn, 2023
#             (hoRbyn4zZ@outlook.com)
# ###################################

    .text
    .code32
    .globl isr_part2
    .globl isr_part3
    .extern __isr

    .long 0

# trampoline routine entry
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
    call *__isr(, %eax, 4)   # addr = $isr + %eax * $4

# trampoline routine exit
isr_part3:
    popal
    popl %gs
    popl %fs
    popl %es
    popl %ds
    addl $8, %esp

    iret
