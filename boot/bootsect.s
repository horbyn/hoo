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

	movw $0xb800, %ax
	movw %ax,     %es
	movw $msg,    %bx # $tag indicates address
	xorw %si,     %si
	movw $0xc,    %cx
	movb $0x0f,   %ah # white font black background
move:
	movb (%bx),   %al
	movw %ax,     %es:(%si)
	incw %bx
	addw $2,      %si
	loop move

	jmp .
msg:
	.string "Hello World!"
.org	0x1fe, 0x90
.word	0xaa55
