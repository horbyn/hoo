# act as a function, `enable_gdt(void *base)`

    .text
    .globl enable_gdt
    .include "kern_will_use.inc"

    .code16
enable_gdt:
    push %ebp
    movl %esp,          %ebp
    # caller-saved(eax, ecx, edx) can be used in callee

    # fast A20 gate
    inb $0x92,          %al
    orb $2,             %al
    outb %al,           $0x92

    # lgdt
    movl %ss:8(%ebp),   %eax
    lgdt (%eax)

    # set CR0.PE
    movl %cr0,          %eax
    orl $1,             %eax
    movl %eax,          %cr0

    ljmp $0x08,         $pm_go  # refresh pipeline
pm_go:
    .code32
    movw $0x10,         %ax
    movw %ax,           %ds
    movw %ax,           %es
    movw %ax,           %fs
    movw %ax,           %gs
    movw %ax,           %ss
    movl $SEG_KSTACK,   %esp

    ret
