#       Copyright (C)    horbyn, 2022
#             (hoRbyn4zZ@outlook.com)

	.text
	.globl start
	.code16

	.set SEC_NR,         3   # 896 # the amount of sector to be loaded (≈448KB)
	.set SEC_144M,       18  # the specification for sector of 1.44M floppy
	.set TRK_144M,       80  # the specification for track of 1.44M floppy
	.set HEAD_144M,      2   # the specification for head of 1.44M floppy

	# system data buffer
    .set MM_KSTACK_BUF,  0x9800
    .set MM_NEW_MBR_SEG, 0x8000
    .set MM_DIED_OFF,    0xfffe
    .set MM_GDT_OFF,     0x104
    .set MM_CURSOR_OFF,  0x100
	.set MEM_ST_OFF,     4   # ARDS struct base address
	.set MEM_NR_OFF,     0   # ARDS amount

	# temp data
	.set TEMP_SYS_SEG,   0x1000

start:
	movw %cs,             %ax
	movw %ax,             %ds
	movw $MM_NEW_MBR_SEG, %ax
	movw %ax,             %ss
	movw $0x7c00,         %sp # stack is set to 0x87c00
	
	## function 1.
	## ds:si -> es:di (0:0x7c00 -> 0x8000:0x7c00)
	cld
	movw $MM_NEW_MBR_SEG, %ax
	movw %ax,             %es
	movl $0x7c00,         %esi
	movl $0x7c00,         %edi
	movl $0x100,          %ecx
	rep  movsw

	ljmp $MM_NEW_MBR_SEG, $go
go:

    ## function 2.
	## Memory detection
	movw $MM_NEW_MBR_SEG, %ax
	movw %ax,             %ds
	movl $0,              %ebx
	movl $0x14,           %ecx
	movw $MEM_ST_OFF,     %di # ARDS struct base address
	movl $0,              MEM_NR_OFF # ARDS amount
detect_mem:
	movl $0xe820,         %eax
	movl $0x534d4150,     %edx
	int  $0x15
	## check
	jnb cf0 # judge cf1? (1 i.e. error)
	jmp  . # error happened
cf0:
	incl MEM_NR_OFF
	addw $0x14,           %di
	cmp  $0,              %ebx
	jne  detect_mem # judge zf1? (1 i.e. detect completed)

	## function 3.
	## kernel loading from disk to memory(0x10000)
	movw $TEMP_SYS_SEG,   %ax
	movw %ax,             %es
	xorw %bx,             %bx # load to 0x1000:0
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
	## function 4.
	## Move gdt to 0x80104
	cld
	movw $MM_NEW_MBR_SEG, %ax
	movw %ax,             %es
	movl $gdt,            %esi
	movl $MM_GDT_OFF,     %edi
	movl $0xa,            %ecx
	rep  movsl

	## function 5.
	## Move the whole kernel from 0x10000 to 0
	movw $TEMP_SYS_SEG,   %ax
	movw %ax,             %ds
	movw $0,              %ax
	movw %ax,             %es
	xorl %esi,            %esi
	xorl %edi,            %edi
	movl $SEC_NR<<7,      %ecx
	rep  movsl

	## function 6.
	## System parameters initialization
	movw $MM_NEW_MBR_SEG, %ax # restore ds
	movw %ax,             %ds
	movl $0,              MM_CURSOR_OFF # global cursor
	movl died,            %eax # died instruction
	movl %eax,            MM_DIED_OFF

	## function 7.
	## Forge a stack environment to call lret
	movl $MM_KSTACK_BUF,     %eax
	movl %eax,               %ss
	movl $0,                 %esp
	movl $MM_NEW_MBR_SEG<<4, %eax
	addl $MM_DIED_OFF,       %eax
	pushl $0x08	# cs seletor in future
	pushl %eax

	## function 8.
	## Enter P.M.
	cli
	inb  $0x92,       %al	# open A20
	orb  $2,          %al
	outb %al,         $0x92

	lgdt gdt_48 

	movl %cr0,        %eax	# enable p.m.
	orl  $1,          %eax
	movl %eax,        %cr0
	
	.code32
	ljmp $0x08,       $0
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
	.long 0x90000 # gdt base

.org	0x1fe, 0x90
.word	0xaa55
