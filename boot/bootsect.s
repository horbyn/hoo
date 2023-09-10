#       Copyright (C)    horbyn, 2023
#             (hoRbyn4zZ@outlook.com)
#
# The features of bootsect are as follow:
# 1. Move to high address itself
# 2. Detect memory
# 3. Load disk
# 4. Move died instruction
# 5. Setup temporary gdt and jump into protected mode

    .text
    .globl _start
    .code16
    .include "kern_will_use.inc"

_start:
    movw $SEG_KSTACK,   %ax
    movw %ax,           %ss
    xorw %sp,           %sp

    # ####            function 1.           ####
    # #### MBR move to high then exec there ####
    .set SEG_MBR,       0x7040
    cld                     # direction increment
    xorw %ax,           %ax
    movw %ax,           %ds
    movw $0x7c00,       %si
    movw $SEG_MBR,      %ax
    movw %ax,           %es
    movw $0x7c00,       %di
    movw $1<<7,         %cx
    rep movsd               # 0:0x7c00 -> 0x7040:0x7c00

    jmp $SEG_MBR,       $still
still:




    # ####    function 2.   ####
    # #### Memory detection ####
    movw $SEG_ARDS,     %ax
    movw %ax,           %ds
    movw %ax,           %es
    movl $0,            OFF_ARDS_CR  # init ards counts
    movw $OFF_ARDS,     %di      # es:di the dest
    movl $0,            %ebx
    movl $0x14,         %ecx
detect_mem:
    movl $0xe820,       %eax
    movl $0x534d4150,   %edx
    int  $0x15

    jnb  cf0 # judge cf1? (1 i.e. error)
    jmp  . # error happened
cf0:
    incl OFF_ARDS_CR
    addw $0x14,         %di
    cmp  $0,            %ebx
    jne  detect_mem # judge zf1? (1 i.e. detect completed)




    # ####              function 3.                ####
    # #### kernel loading from disk to mm.(0x1000) ####
    .set SEG_KERN,      0x100
    .set SEC,           63  # the specification for sector of boot device
    .set HEAD,          16  # the specification for head of boot device
    .set SEC_CR,        896 # the amount of sector to be loaded (≈448KB)
    movw %cs,           %ax
    movw %ax,           %ds
    movw $SEG_KERN,     %ax
    movw %ax,           %es
    xorw %bx,           %bx # load to 0x100:0

    movb $HEAD,         %dl
    movb $SEC,          %al
    mulb %dl # result is in %ax
    movw %ax,           sector_mul_head

load_sect:
    movw lba_base,      %ax
    movb $SEC,          %dl
    divb %dl
    addb $1,            %ah
    movb %ah,           %cl # sector number (cl bit-0~5)

    xorw %dx,           %dx
    movw lba_base,      %ax
    divw sector_mul_head
    movb %al,           %ch # track number--ch bit-0~5
    andb $0x3f,         %ch
    andb $0xc0,         %al
    orb  %al,           %cl # track number--cl bit-6~7 (track number less than 130)

    movw %dx,           %ax
    movb $SEC,          %dl
    divb %dl
    movb %al,           %dh # head number

after_reset:
    movb $0x80,         %dl # floppya driver number is 0
    movb $0x02,         %ah
    movb $1,            %al # load 1 sector every time
    int  $0x13

    jnb  1f # judge cf0 (No error happened)
    hlt                     # failure
1:
    cmpw $SEC_CR,       lba_base
    jb   2f # judge cf1 (i.e. $SEC_CR > lba_base)
    jmp  load_sect_ok # othervise completed (lba_base >= $SEC_CR)
2:
    addw $0x200,        %bx
    cmpw $0,            %bx
    jnz  3f
    movw %es,           %bx
    addw $0x1000,       %bx
    movw %bx,           %es
    xorw %bx,           %bx
3:
    addw $0x1,          lba_base
    jmp  load_sect

load_sect_ok:
    # move kernel to 0
    # the magic value that act as address below
    #    is corresponding to the memory layout
    #    in `kern_will_use.inc`
    movw $SEG_KERN,     %ax
    movw %ax,           %ds
    xorw %ax,           %ax
    movw %ax,           %es
    cld

move:
    xorw %si,           %si
    xorw %di,           %di
    cmpw $0x7000,       %ax
    jz last
    movw $0x4000,       %cx # move $0x1_0000 bytes
    jmp move_cont
last:
    movw $0x1c00,       %cx # move $0x7000 bytes last
move_cont:
    rep
    movsd

    addw $0x1000,       %ax
    cmpw $0x8000,       %ax
    jz move_end
    movw %ax,           %es
    movw %ds,           %bx
    addw %ax,           %bx
    movw %bx,           %ds
    jmp move
move_end:




    # ####      function 4.      ####
    # #### Move died instruction ####
    cld
    movw %cs,           %ax
    movw %ax,           %ds
    movw $died,         %si
    movw %ax,           %es
    movw $0x7bfc,       %di
    movsw                   # move DIED INSTRUCTION to 0x77ffc




    # #### function 5. ####
    # #### enter p.m.  ####
    cli
    movw %cs,           %ax
    movw %ax,           %ds
    inb  $0x92,         %al     # open A20
    orb  $2,            %al
    outb %al,           $0x92

    lgdt gdt_48

    movl %cr0,          %eax    # enable p.m.
    orl  $1,            %eax
    movl %eax,          %cr0

    # #### JUMP INTO KERNEL ####
    ljmp $0x08,         $0
    # ###### BOOT ENDING #######




died:
    jmp .

lba_base:
    .word 0x1   # loading from no.2 sector (i.e., LBA is no.1)
sector_mul_head:
    .word 0
boot_gdt:
	.quad 0x0000000000000000
	.quad 0x00cf9a000000ffff # exe, no-readable, no-conform
	.quad 0x00cf92000000ffff # no-exe, no-writable, down
gdt_48:
	.word .-boot_gdt
	.long SEG_MBR<<4 + 0x7c00 + (boot_gdt - _start)

.org    0x1fe, 0x90
.word   0xaa55
