#       Copyright (C)    horbyn, 2022
#             (hoRbyn4zZ@outlook.com)

    .text
    .globl _start
    .code16

	.set SEC_NR,       6        # 896 # the amount of sector to be loaded (≈448KB)
	.set SEC_144M,     18       # the specification for sector of 1.44M floppy
	.set TRK_144M,     80       # the specification for track of 1.44M floppy
	.set HEAD_144M,    2        # the specification for head of 1.44M floppy

	# system data buffer
    .set SEG_KERN,     0x1000
    .set SEG_MBR,      0xf00
    .set OFF_GDT,      0x198    # gdt base
    .set OFF_ARDS,     8        # ARDS struct base address
    .set OFF_ARDS_CR,  4        # ARDS amount
    .set OFF_CURSOR,   0        # cursor location
    .set SEG_STACK,    0
    .set STACK_SP,     0xf000
    .set OFF_DIED,     0x77fc   # died instruction

_start:
	movw $SEG_MBR,     %ax
	movw %ax,          %ds
	movw $SEG_STACK,   %ax
	movw %ax,          %ss
	movw $STACK_SP,    %sp      # stack is set to 0:0xf000

	# ####    function 1.   ####
	# #### Memory detection ####
    movl $0,           OFF_ARDS_CR  # init ards counts
    movw $SEG_MBR,     %ax
    movw %ax,          %es
    movw $OFF_ARDS,    %di      # es:di the dest
	movl $0,           %ebx
	movl $0x14,        %ecx
detect_mem:
    movl $0xe820,      %eax
    movl $0x534d4150,  %edx
    int  $0x15

    jnb  cf0 # judge cf1? (1 i.e. error)
    jmp  . # error happened
cf0:
    incl OFF_ARDS_CR
    addw $0x14,        %di
    cmp  $0,           %ebx
    jne  detect_mem # judge zf1? (1 i.e. detect completed)

    # ####              function 2.                 ####
    # #### kernel loading from disk to mm.(0x10000) ####
	movw %cs,          %ax
	movw %ax,          %ds
    movw $SEG_KERN,    %ax
    movw %ax,          %es
    xorw %bx,          %bx # load to 0x1000:0
load_sect:
    movw lba_base,        %ax
	movb $SEC_144M,       %dl
	divb %dl
	addb $1,              %ah
	movb %ah,             %cl # sector number (cl bit-0~5)
	andb $1,              %al # judge odd/even
	je   head0 # judge zf1? (odd & 1 yields 1, zf0; even & 1 yields 0, zf1)
head1:
	movb $1,              %dh # head number
	jmp  1f
head0:
	movb $0,              %dh # head number
1:
	movw lba_base,        %ax
	movb $SEC_144M<<1,    %dl
	divb %dl
	movb %al,             %ch # track number--ch bit-0~5
	andb $0x3f,           %cl # track number--cl bit-6~7 (track number less than 80)
after_reset:
	movb $0,              %dl # floppya driver number is 0
	movb $0x02,           %ah
	movb $1,              %al # load 1 sector every time
	int  $0x13

	jnb  2f # judge cf0 (No error happened)
	movb $0,              %dl
	movw $0,              %ax
	int  $0x13 # reset floppy driver due to fail
	jmp  after_reset
2:
	cmpw $SEC_NR,         lba_base
	jb   3f # judge cf1 (i.e. $SEC_NR > lba_base)
	jmp  load_sect_ok # othervise completed (lba_base >= $SEC_NR)
3:
	addw $0x200,          %bx
	addw $0x1,            lba_base
	jmp  load_sect

load_sect_ok:
    # ####     function 3.     ####
    # #### System parameters initialization ####
    cld
    movw %cs,             %ax
    movw %ax,             %ds
    movw $SEG_MBR,        %ax
    movw %ax,             %es
	movl $gdt,            %esi
	movl $OFF_GDT,        %edi
	movl $0xa,            %ecx
	rep  movsl # Move gdt to 0xf198
    movl died,           %eax
    movl %eax,           OFF_DIED # died instruction
	movw $SEG_MBR,       %ax
	movw %ax,            %ds
    movl $0,             OFF_CURSOR # global cursor location

    # #### function 4. ####
    # #### Forge a stack environment to call lret
    pushl $0x08	# cs seletor in future
    pushl $OFF_DIED

    # #### function 5. ####
    # #### enter p.m. ####
    cli
	movw %cs,             %ax
    movw %ax,             %ds
	inb  $0x92,              %al	# open A20
	orb  $2,                 %al
	outb %al,                $0x92

	lgdt gdt_48 

	movl %cr0,               %eax	# enable p.m.
	orl  $1,                 %eax
	movl %eax,               %cr0

    ljmp $0x08, $pm_go # refresh pipeline

pm_go:
    .code32
	movw $0x10,              %ax
	movw %ax,                %ds
	movw %ax,                %es
	movw %ax,                %fs
	movw %ax,                %gs
	movw %ax,                %ss

    ljmp $0x08, $SEG_KERN<<4
    # ######### end boot ###########

died:
	jmp  .

lba_base:
	.word 0x1 # loading from no.2 sector (i.e., LBA is no.1)
gdt:
	.quad 0x0000000000000000
	.quad 0x00cf9a000000ffff # exe, no-readable, no-conform
	.quad 0x00cf92000000ffff # no-exe, no-writable, down
	.quad 0x0000000000000000
	.quad 0x0000000000000000
gdt_48:
	.word .-gdt
	.long 0xf198 # gdt base(SEG_MBR<<4 + OFF_GDT)

.org	0x1fe, 0x90
.word	0xaa55
