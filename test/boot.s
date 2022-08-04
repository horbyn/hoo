    .globl _start
    .code16

    .set CYL_144M, 80
    .set HEA_144M, 2
    .set SEC_144M, 18
    .set NUM_SEC,  3

    .text
lba_base:
    .word 1
mm_addr:
    .word 0x1000
_start:
    movw %cs,     %ax
    movw %ax,     %ds
    movw %ax,     %es
    movw %ax,     %ss
    movw $0x7c00, %ax
    movw %ax,     %sp

    # test int 0x13, ah = 2
    # loading from floppy to mem
load_sect:
    movw lba_base,     %ax
    movb $SEC_144M<<1, %bl
    divb %bl
    movb %al,          %ch  # C
    movw lba_base,     %ax
    movb $SEC_144M,    %bl
    divb %bl
    andb $1,           %al
    jz head0                # jump if (zf == 0)

head1:
    movb $1, %dh            # H
    jmp 1f

head0:
    movb $0, %dh            # H

1:
    xorb %cl, %cl
    addb $1,  %ah
    movb %ah, %cl           # S

after_reset:
    movb $1,      %al
    movb $0,      %dl
    movw mm_addr, %bx
    movb $2,      %ah
    int  $0x13              # 0x13, ah2 = read disk to mm

    jnb  2f                 # jump if (cf == 0)
    movb $0, %dl
    movw $0, %ax
    int  $0x13              # 0x13, ah0 = reset disk driver
    jmp  after_reset

2:
    cmpw $NUM_SEC, lba_base # if (NUM_SEC > lba_base) then (cf = 1)
    jb   3f                 # jump if (cf == 1)
    jmp  load_sect_ok

3:
    addw $0x200, mm_addr
    addw $1,     lba_base
    jmp  load_sect

load_sect_ok:
    ljmp $0, $0x1000        # jump to kernel
    jmp  .

    .org  0x1fe, 0x90
    .word 0xaa55
