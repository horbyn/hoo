    .globl _start
    .code16
    .text
_start:
    movl %cs,    %eax
    movl %eax,   %ds
    movl %eax,   %es
    movl %eax,   %ss
    movl $stack, %eax
    movl %eax,   %esp

    ljmp $0,     $0x50

    .org 0x100, 0x90
stack:
    .data
.string "hello world!"
