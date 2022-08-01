    .code16

    .text
    movw %cs,     %ax
    movw %ax,     %ds
    movw %ax,     %es
    movw %ax,     %ss
    movw $stack2, %ax
    movw %ax,     %sp

    .org 0x50, 0x1234
stack2:

    .data
.string "hello"

    .bss
j:
.quad   # 八字节
