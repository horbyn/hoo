    .text
    .code32
    .globl isr_part1
    .extern isr
.macro ISRNOERR id
isr_part1_\id:
    pushl $0
    pushl $\id
    jmp isr_part2
.endm

.macro ISRERR id
isr_part1_\id:
    pushl $\id
    jmp isr_part2
.endm

ISRNOERR 0
ISRNOERR 1
ISRNOERR 2
ISRNOERR 3
ISRNOERR 4
ISRNOERR 5
ISRNOERR 6
ISRNOERR 7
ISRERR   8
ISRNOERR 9
ISRERR   10
ISRERR   11
ISRERR   12
ISRERR   13
ISRERR   14
ISRNOERR 15
ISRNOERR 16
ISRERR   17
ISRNOERR 18
ISRNOERR 19
ISRNOERR 20
ISRNOERR 21
ISRNOERR 22
ISRNOERR 23
ISRNOERR 24
ISRNOERR 25
ISRNOERR 26
ISRNOERR 27
ISRNOERR 28
ISRNOERR 29
ISRNOERR 30
ISRNOERR 31

isr_part1:
    .long isr_part1_0,  isr_part1_1,  isr_part1_2
    .long isr_part1_3,  isr_part1_5,  isr_part1_5
    .long isr_part1_6,  isr_part1_7,  isr_part1_8
    .long isr_part1_9,  isr_part1_10, isr_part1_11
    .long isr_part1_12, isr_part1_13, isr_part1_14
    .long isr_part1_15, isr_part1_16, isr_part1_17
    .long isr_part1_18, isr_part1_19, isr_part1_20
    .long isr_part1_21, isr_part1_22, isr_part1_23
    .long isr_part1_24, isr_part1_25, isr_part1_26
    .long isr_part1_27, isr_part1_28, isr_part1_29
    .long isr_part1_30, isr_part1_31

isr_part2:
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs
    pushl %ss
    pushal

    movl 52(%esp), %eax
    call *isr(, %eax, 4)   # addr = $isr + %eax * $4
    addl $4, %esp

    popal
    addl $4, %esp           # jump %ss
    popl %gs
    popl %fs
    popl %es
    popl %ds
    addl $8, %esp

    iret
