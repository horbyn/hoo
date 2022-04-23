    .text
    # .globl: https://sourceware.org/binutils/docs/as/Global.html#Global
    .globl _start
    # .code16: https://sourceware.org/binutils/docs/as/i386_002d16bit.html#i386_002d16bit
    .code16
_start:
    jmp .

    # .org: https://sourceware.org/binutils/docs/as/Org.html#Org
    .org 0x1fe, 0x90
    .word 0xaa55
