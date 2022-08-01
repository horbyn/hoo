    .globl _start
    .code16

    .text
_start:
    movw %cs,     %ax
    movw %ax,     %ds
    movw %ax,     %es
    movw %ax,     %ss
    movw $stack1, %ax
    movw %ax,     %sp

    .org 0x50, 0x90
stack1:

    .data
.string "hello world!"

    .bss
i:
.long   # 四字节
