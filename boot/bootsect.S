#include "config_boot.h"

    .text
    .globl _start
    .code16

_start:
    movw $SEG_KSTACK,   %ax
    movw %ax,           %ss
    xorw %sp,           %sp

    # 功能一：将 MBR 移动到高地址处并在高地址执行
    .set SEG_MBR,       0x7240
    cld
    xorw %ax,           %ax
    movw %ax,           %ds
    movw $0x7c00,       %si
    movw $SEG_MBR,      %ax
    movw %ax,           %es
    movw $0x7c00,       %di
    movw $1<<7,         %cx
    rep movsd               # 0:0x7c00 -> 0x7240:0x7c00

    jmp $SEG_MBR,       $still
still:




    # 功能二：内存检测
    movw $SEG_ARDS,     %ax
    movw %ax,           %ds
    movw %ax,           %es
    movl $0,            OFF_ARDS_CR # 初始化 ARDS 计数
    movw $OFF_ARDS,     %di         # es:di 表示目的地址
    movl $0,            %ebx
    movl $0x14,         %ecx
detect_mem:
    movl $0xe820,       %eax
    movl $0x534d4150,   %edx
    int  $0x15

    jnb  cf0 # 判断 CF=1？（1 表示出错）
    jmp  .
cf0:
    incl OFF_ARDS_CR
    addw $0x14,         %di
    cmp  $0,            %ebx
    jne  detect_mem # 判断 ZF=1？（1 表示检测完成）




    # 功能三：从硬盘将内核加载到内存 0x1000
    .set SEG_KERN,      0x100
    .set SEC_144M,      18  # 1.44M 软盘的扇区规格
    .set SEC_CR,        896 # 将要加载的扇区数量（≈448KB）
    movw %cs,           %ax
    movw %ax,           %ds
    movw $SEG_KERN,     %ax
    movw %ax,           %es
    xorw %bx,           %bx # 加载到 0x100:0
load_sect:
    movw lba_base,      %ax
    movb $SEC_144M,     %dl
    divb %dl
    addb $1,            %ah
    movb %ah,           %cl # 扇区号（放在 cl bit0 至 bit5）
    andb $1,            %al # 判断奇偶
    je   head0 # 判断 ZF=1？（0 表示奇 & 1；1 表示偶 & 1）
head1:
    movb $1,            %dh # 磁头号
    jmp  1f
head0:
    movb $0,            %dh # 磁头号
1:
    movw lba_base,      %ax
    movb $SEC_144M<<1,  %dl
    divb %dl
    movb %al,           %ch # 磁道号（放在 ch bit0 至 bit5）
    andb $0x3f,         %cl # 磁道号（放在 cl bit6 至 bit7）
after_reset:
    movb $0,            %dl # 软盘驱动号是 0
    movb $0x02,         %ah
    movb $1,            %al # 每次加载一个扇区
    int  $0x13

    jnb  2f # 判断 CF=0？（0 表示没出错）
    movb $0,            %dl
    movw $0,            %ax
    int  $0x13 # 因出错而重置软盘驱动
    jmp  after_reset
2:
    cmpw $SEC_CR,       lba_base
    jb   3f             # 判断 CF=1？（1 表示 $SEC_CR > lba_base）
    jmp  load_sect_ok   # 否则完成（lba_base >= $SEC_CR）
3:
    addw $0x200,        %bx
    cmpw $0,            %bx
    jnz  4f
    movw %es,           %bx
    addw $0x1000,       %bx
    movw %bx,           %es
    xorw %bx,           %bx
4:
    addw $0x1,          lba_base
    jmp  load_sect

load_sect_ok:
    # 将内核移动到内存 0，下面出现的 magic number 对应于 `config_boot.h` 中的内存布局
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
    movw $0x4000,       %cx # 移动 0x1_0000 字节
    jmp move_cont
last:
    movw $0x1c00,       %cx # 移动最后的 0x7_000 字节
move_cont:
    rep
    movsd

    addw $0x1000,       %ax
    cmpw $0x8000,       %ax
    jz move_end
    movw %ax,           %es
    movw %ds,           %bx
    addw $0x1000,       %bx
    movw %bx,           %ds
    jmp move
move_end:




    # 功能四：移动 DIED 指令
    cld
    movw %cs,           %ax
    movw %ax,           %ds
    movw $died,         %si
    movw %ax,           %es
    movw calc_off_died, %di
    movsw                   # 将 DIED 指令移动到 0x77ffc




    # 功能五：进入保护模式
    cli
    movw %cs,           %ax
    movw %ax,           %ds
    inb  $0x92,         %al     # 打开 A20 地址线
    orb  $2,            %al
    outb %al,           $0x92

    lgdt gdt_48

    movl %cr0,          %eax    # 进入保护模式
    orl  $1,            %eax
    movl %eax,          %cr0




    # 功能六：开启分页
    movl $0x100,        %ecx
    movl $0,            %eax
    movl pgtable_addr,  %ebx
pgtable:
    movl %eax,          %edx
    orl  $7,            %edx
    movl %edx,          (%ebx)
    addl $0x1000,       %eax
    addl $4,            %ebx
    loop pgtable

    movl $SEG_PGTABLE<<4, %eax
    orl  $7,            %eax
    movl pdtable_addr,  %ebx
    movl %eax,          (%ebx)
    addl $PDE_HIGH_OFF, %ebx
    movl %eax,          (%ebx)
    movl $SEG_PDTABLE<<4, %eax
    orl  $7,            %eax
    addl $PDE_LAST_OFF, %ebx
    movl %eax,          (%ebx)

    movl $SEG_PDTABLE<<4, %eax
    movl %eax,          %cr3
    movl %cr0,          %eax
    orl  $0x80000000,   %eax
    movl %eax,          %cr0

    # ####### 跳入内核 #######
    ljmp $0x08,         $0
    # ###### boot 结束 #######




died:
    jmp .

lba_base:
    .word 0x1   # 从扇区号 2 开始加载（因为 LBA 从 1 开始）
calc_off_died:
    .long SEG_DIED<<4 + OFF_DIED - SEG_MBR<<4
pdtable_addr:
    .long SEG_PDTABLE<<4 - SEG_MBR<<4
pgtable_addr:
    .long SEG_PGTABLE<<4 - SEG_MBR<<4
boot_gdt:
	.quad 0x0000000000000000
	.quad 0x00cf9a000000ffff # exe, no-readable, no-conform
	.quad 0x00cf92000000ffff # no-exe, no-writable, down
gdt_48:
	.word .-boot_gdt
	.long SEG_MBR<<4 + 0x7c00 + (boot_gdt - _start)

.org    0x1fe, 0x90
.word   0xaa55
