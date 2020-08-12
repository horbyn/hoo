#       Copyright (C)    horbyn, 2020
#             (hoRbyn4zZ@outlook.com)
#
# The function of bootsect are as follow:
# 1. Move to high address itself
# 2. Detect memory
# 3. Load kernel from disk

	.text
	.globl start
	.code16

	.set MEM_NR,       0   # ARDS amount
	.set MEM_ST,       4   # ARDS struct base address
	.set SEC_NR,       2   # 896 # the amount of sector to be loaded (≈448KB)
	.set SEC_144M,     18  # the specification for sector of 1.44M floppy
	.set TRK_144M,     80  # the specification for track of 1.44M floppy
	.set HEAD_144M,    2   # the specification for head of 1.44M floppy

start:
	movw %cs,         %ax
	movw %ax,         %ds
	movw $0x8000,     %ax
	movw %ax,         %ss
	movw $0x7c00,     %sp # stack is set to 0x87c00

	## clear screen
	movw $0x600,      %ax
	movw $0,          %bx
	movw $0,          %cx
	movw $0x184f,     %dx
	int  $0x10

	## function 1.
	## ds:si -> es:di (0:0x7c00 -> 0x8000:0x7c00)
	cld
	movw $0x100,      %cx
	movw $0x7c00,     %si
	movw $0x8000,     %ax
	movw %ax,         %es
	movw $0x7c00,     %di
	rep movsw

	ljmp $0x8000,     $go
go:

	## function 2.
	movw $0x8000,     %ax
	movw %ax,         %ds
	movl $0,          %ebx
	movl $0x14,       %ecx
	movw $MEM_ST,     %di # ARDS struct base address
	movl $0,          MEM_NR # ARDS amount
detect_mem:
	movl $0xe820,     %eax
	movl $0x534d4150, %edx
	int  $0x15
	## check
	jnb cf0 # judge cf1? (1 i.e. error)
	jmp . # error happened
cf0:
	incl MEM_NR
	addw $0x14,       %di
	cmp  $0,          %ebx
	jne detect_mem # judge zf1? (1 i.e. detect completed)

	## function 3.
	## 16bit/8bit less than 65535/255
	movw $0x1000,     %ax
	movw %ax,         %es
	xorw %bx,         %bx # load to 0x1000:0
load_sect:
	movw lba_base,    %ax
	movb $SEC_144M,   %dl
	divb %dl
	addb $1,          %ah
	movb %ah,         %cl # sector number (cl bit-0~5)
	andb $1,          %al # judge odd/even
	je   head0 # judge zf1? (odd & 1 yields 1, zf0; even & 1 yields 0, zf1)
head1:
	movb $1,          %dh # head number
	jmp  1f
head0:
	movb $0,          %dh # head number
1:
	movw lba_base,    %ax
	movb $SEC_144M<<1,%dl
	divb %dl
	movb %al,         %ch # track number--ch bit-0~5
	andb $0x3f,       %cl # track number--cl bit-6~7 (track number less than 80)
after_reset:
	movb $0,          %dl # floppya driver number is 0
	movb $0x02,       %ah
	movb $1,          %al # load 1 sector every time
	int  $0x13

	jnb  2f # judge cf0 (No error happened)
	movb $0,          %dl
	movw $0,          %ax
	int  $0x13 # reset floppy driver due to fail
	jmp  after_reset
2:
	cmpw $SEC_NR,     lba_base
	jb   3f # judge cf1 (i.e. $SEC_NR > lba_base)
	jmp  load_sect_ok # othervise completed (lba_base >= $SEC_NR)
3:
	addw $0x200,      %bx
	addw $0x1,        lba_base
	jmp  load_sect
load_sect_ok:
	ljmp $0x1000,     $0

lba_base:
	.word 0x1 # loading from no.2 sector (i.e., LBA is no.1)

.org	0x1fe, 0x90
.word	0xaa55
