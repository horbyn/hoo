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
start:
	movw %cs,     %ax
	movw %ax,     %ds

	# clear screen
	movw $0x600,  %ax
	movw $0,      %bx
	movw $0,      %cx
	movw $0x184f, %dx
	int  $0x10

	# function 1.
	# ds:si -> es:di (0:0x7c00 -> 0x8000:0x7c00)
	cld
	movw $0x100,  %cx
	movw $0x7c00, %si
	movw $0x8000, %ax
	movw %ax,     %es
	movw $0x7c00, %di
	rep movsw

go:
	ljmp $0x8000, $go

	######

	jmp .
msg:
	.string "Hello World!"
.org	0x1fe, 0x90
.word	0xaa55
