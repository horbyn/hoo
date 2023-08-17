# #######################################################
#                           Copyright (C)    horbyn, 2023
#                                 (hoRbyn4zZ@outlook.com)
#
#
# NOTE:
# ref to: https://stackoverflow.com/a/2538212/21945541
#
#     In x86-32 parameters for Linux system call are passed
# using registers. %eax for syscall_number. %ebx, %ecx,
# %edx, %esi, %edi, %ebp are used for passing 6 parameters
# to system calls
#
# ref to: https://web.archive.org/web/20120822144129/http://
# www.cin.ufpe.br/~if817/arquivos/asmtut/index.html#syscalls
#
#     The first argument goes in EBX, the second in ECX,
# then EDX, ESI, EDI, and finally EBP
#
#     For simplicity, here i use 6 parameters at most
# #######################################################

    .text
    .code32
    .extern stub
    .globl syscall
    .globl printf

# 80th interrupt handling routine
syscall:
    pushl %ebp
    movl %esp, %ebp
    popl %ebp
    ret

# ################################# #
#                                   #
# @brief formatting output          #
#                                   #
# @param format:    format string   #
# @param ...:       more parameters #
#                                   #
# ################################# #
printf:
    pushl %ebp
    movl %esp,          %ebp
    pushal                                                  # save user context

    movl $1,            %eax
    lea (%ebp, %eax, 8),%ebx
    movl $0,            %eax
    int $0x80

    popal                                                   # restore user context
    popl %ebp
    ret
