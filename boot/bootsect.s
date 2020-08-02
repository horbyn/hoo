#       Copyright (C)    horbyn, 2020
#             (hoRbyn4zZ@outlook.com)
#
# The function of bootsect are as follow:
# 1. Move to high address itself
# 2. Detect memory
# 3. set GDT

	.text
	.globl start
	.code16

	.set MEM_NR, 0 # ARDS amount
	.set MEM_ST, 4 # ARDS struct base address

start:
	movw %cs,     %ax
	movw %ax,     %ds

	## clear screen
	movw $0x600,  %ax
	movw $0,      %bx
	movw $0,      %cx
	movw $0x184f, %dx
	int  $0x10

	## function 1.
	## ds:si -> es:di (0:0x7c00 -> 0x8000:0x7c00)
	cld
	movw $0x100,  %cx
	movw $0x7c00, %si
	movw $0x8000, %ax
	movw %ax,     %es
	movw $0x7c00, %di
	rep movsw

	ljmp $0x8000, $go
go:

	## function 2.
	movw $0x8000, %ax
	movw %ax,     %ds
	movl $0,      %ebx
	movl $0x14,   %ecx
	movw $MEM_ST, %di # ARDS struct base address
	movl $0,      MEM_NR # ARDS amount
detect_mem:
	movl $0xe820,     %eax
	movl $0x534d4150, %edx
	int  $0x15
	## check
	jnb cf0 # judge cf1? (1 i.e. error)
	jmp . # error happened
cf0:
	incl MEM_NR
	addw $0x14,   %di
	cmp  $0,      %ebx
	jne detect_mem # judge zf1? (1 i.e. detect completed)

	jmp .
msg:
	.string "Hello World!"
.org	0x1fe, 0x90
.word	0xaa55
