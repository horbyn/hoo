# Segment v.s. Section

参考资料：

- [《操作系统真象还原，郑钢》](https://book.douban.com/subject/26745156/)

个人理解是，两者都是指逻辑段。平时汇编源文件里说的代码 *"段"*，数据 *"段"* 等，都是指 `section`；而 `segment` 指的是链接之后的整个可执行文件内部的一个个逻辑段。类似以下：

```cpp
/*
 * 汇编文件1            汇编文件2
 * ┌────────────────┐   ┌────────────────┐
 * │section: .text1 │   │section: .text2 │
 * ├────────────────┤   ├────────────────┤
 * │section: .data1 │   │section: .data2 │
 * └────────────────┘   └────────────────┘
 *            \          /
 *             \ 链接   /
 *           ┌───────────────┐
 *           │segment: .text │
 *           ├───────────────┤
 *           │segment: .data │
 *           └───────────────┘
 */
```

如果你写过汇编，你可能知道汇编器会提供各种各样用来生成逻辑段的伪指令（`directive`），有的汇编器可能用的关键字是 `section`，有的可能是 `segment` 或者其他什么词。但这个只是汇编源文件层次上的事情，本质上只是方便程序员划分程序逻辑的 *"逻辑段"* 而已。**这种源文件上的逻辑段，一般用 `section` 来指代，中文常译为 `节`**。比如上面这个例子，汇编文件 1 和 2 都定义了逻辑段，因此广义上的 `section` 指的是 `.text1`、`.text2`、`.data1` 和 `.data2`

编译型语言从源文件走到可执行文件这个过程往往涉及多个阶段：源文件编译得到汇编文件，汇编文件汇编得到目标文件，最终将所有用到的目标文件链接到一起，就是可执行文件。可执行文件里面也有逻辑段，但这里的 *"逻辑"* 不是面向程序员的，而是面向加载器的。可执行文件包含的二进制数据可能是指令，可能是数据，也可能是有其他用途的东西。所以这里的 *"逻辑"* 指的是可读、可写、可执行，链接阶段负责将具有相同 *"逻辑"* 的节（`section`）合并起来。**这种可执行文件上的逻辑段，一般用 `segment` 来指代，中文常译为 `段`**。比如上面那个例子，链接完成后，得到了一个文件，里面的 `.text` 和 `.data` 就是广义上说的 `segment`。这个例子里 `.text` 由 `.text1` 和 `.text2` 合并而来，`.data` 同理（但是，也完全有可能和直观上不同，比如 `.text1` 和 `.data1` 合并，这是链接器负责的事，它的逻辑怎样的那需要研究链接器才行）

但也要注意，无论是 "节（`section`）" 还是 "段（`segment`）"，本质都是逻辑段，作区分只是为了强调是输入文件（汇编源文件）还是输出文件（目标文件）。实际上无论是输入文件还是输出文件，完全可以使用同一个术语，因为都是逻辑段。所以，具体语境具体分析

<br></br>

# `.iplt` 这些是什么节

参考 [Re: Why gcc manual does not explain some sections](https://gcc.gnu.org/legacy-ml/gcc-help/2011-02/msg00172.html)

无论是输入文件的节（`section`）或是输出文件的段（`segment`），常见的都是 `.text`（代码段）、`.data`（数据段）和 `.bss`（bss 段）

但是当你真正经过 gcc 编译，ld 链接（指定 `-Map` 选项）后，会得到一个 `.map` 的文件，里面是输出文件的内存布局。观察这个文件你会发现多了许多不认识/没见过的段

输入文件如下：

```asm
############ 第一个输入文件 ############
    .globl _start
    .code16

    .text
_start:
    movw %cs,     %ax
    movw %ax,     %ds
    movw %ax,     %es
    movw %ax,     %ss
    movw $stack1, %ax
    movw %ax,     %sp

    .org 0x50, 0x90
stack1:

    .data
.string "hello world!"

    .bss
i:
.long   # 四字节



############ 第一个输入文件 ############
    .code16

    .text
    movw %cs,     %ax
    movw %ax,     %ds
    movw %ax,     %es
    movw %ax,     %ss
    movw $stack2, %ax
    movw %ax,     %sp

    .org 0x50, 0x1234
stack2:

    .data
.string "hello"

    .bss
j:
.quad   # 八字节
```

这两个输入文件没什么真正的含义，仅仅是显式标明这里有 `.text`、`.data` 和 `.bss`，这些程序段是我们显式给出的

以下是编译和链接命令

```shell
as --32 test1.s -o test1.o
as --32 test2.s -o test2.o
ld -m elf_i386 -T test.lds -Map test_map test1.o test2.o -o test
```

汇编文件里的 `.code16`，以及编译命令里面的 `--32` 或 `-m elf_i386` 等等可以不用理会，仅仅是指定了目标平台而已，这里只需关心生成的 `.map` 文件即可，如下：

```map

Memory Configuration

Name             Origin             Length             Attributes
*default*        0x0000000000000000 0xffffffffffffffff

Linker script and memory map

                0x0000000000000010                . = 0x10

.text           0x0000000000000010       0xa0
 *(.text)
 .text          0x0000000000000010       0x50 test1.o
                0x0000000000000010                _start
 .text          0x0000000000000060       0x50 test2.o

.iplt           0x00000000000000b0        0x0
 .iplt          0x00000000000000b0        0x0 test1.o

.rel.dyn        0x00000000000000b0        0x0
 .rel.got       0x00000000000000b0        0x0 test1.o
 .rel.iplt      0x00000000000000b0        0x0 test1.o
                0x0000000000000200                . = 0x200

.data           0x0000000000000200       0x13
 *(.data)
 .data          0x0000000000000200        0xd test1.o
 .data          0x000000000000020d        0x6 test2.o

.got            0x0000000000000214        0x0
 .got           0x0000000000000214        0x0 test1.o

.got.plt        0x0000000000000214        0x0
 .got.plt       0x0000000000000214        0x0 test1.o

.igot.plt       0x0000000000000214        0x0
 .igot.plt      0x0000000000000214        0x0 test1.o
                0x0000000000000218                . = ALIGN (0x8)

.bss            0x0000000000000218        0x0
 *(.bss)
 .bss           0x0000000000000218        0x0 test1.o
 .bss           0x0000000000000218        0x0 test2.o
LOAD test1.o
LOAD test2.o
OUTPUT(test elf32-i386)
```

输出文件的 `.text` 加载地址（`LMA`）开始于 `0x10`，以及 `.data` 加载地址开始于 `0x200` 可以不用管，这里是链接脚本的影响

可以发现，实际上除了我们在编程时指定的 `.text`、`.data` 和 `.bss` 外，还生成了其他程序段

根据参考资料 [探索 GOT 和 PLT](https://systemoverlord.com/2017/03/19/got-and-plt-for-pwning.html)，这里总结一下：

- `plt`：可链接的程序表（`Procedure Linkage Table`），用来寻找 `.got.plt` 里的地址
- `iplt`：符号 `STT_GNU_IFUNC` 使用这个段
- `.rel.dyn`：动态链接器用来保存动态的可重定位符号
    + `rel.got` 和 `rel.iplt` 也属于 `.rel.dyn`，功能类似，只是名字不同
- `.got`：全局偏移表（`Global Offset Table`），根据这个表为外部符号进行重定位
- `.got.plt`：这是为 `PLT` 提供的 `GOT`，通常是 `.got` 的一部分
- `.igot.plt`：也是符号 `STT_GNU_IFUNC` 使用这个段
- `.shstrtab`：保存节名
- `.symtab`：保存符号表
- `.strtab`：保存符号名字

<br></br>

# VMA v.s. LMA

LMA（加载地址）：运行阶段之前，程序段从存储系统搬到内存（即加载），LMA 就是这个过程的目的地址

VMA（虚拟内存地址）：运行阶段时，程序段的地址

对于普通 PC 来说，两个地址是相等的——程序加载到哪，就从哪里开始执行

但对于嵌入式设备（特别是 RAM 偏小的），这两个地址才可能不同。一般 ROM（Flash）较大而 RAM 较小，所以只希望让数据加载到 RAM，因为数据有写入权限；而代码直接运行在 ROM 就行。所以在加载的时候可以将整个可执行文件都加载到 ROM，但在运行阶段需要将数据段移动到 RAM——也即是数据段的 LMA 和 VMA 是不同的

<br></br>

# ELF 文件

对任意一个 `ELF` 文件执行

```shell
readelf -a xxx.elf
```

这样的命令，可以看到输出大致如下：

```shell
ELF Header:
    ...
Section Headers:
    ...
Program Headers:
    ...
 Section to Segment mapping:
    ...
Symbol table '.symtab' contains XX entries:
    ...
```

当然还有其他如动态段（`dynamic section`）或者可重定向符号（`relocation`）等等，但这里只关注主要的部分

- `ELF 头（ELF Header）`：或者说 `ELF 文件头`，位于 `ELF` 文件最开始，包含整个文件的结构信息
- `节头表（Section Header）`：节指的是输入文件的逻辑段，所以这个表专门用于链接阶段，每个节包含指令、符号等等
- `程序头表（Program Header）`：也称为 `段头表（segment）`：段指的是输出文件的逻辑段，所以这个表专门用于运行阶段，告诉系统如何创建进程的内存映像
- `映射（Section to Segment mapping）`：输出段包含了哪些输入节
- `符号表（Symbol table）`：保存了程序所有实现的/使用的变量/函数，一般由 `.symtab` 包含符号表（也可以用 `nm` 来查看符号表）

<br></br>

# 链接脚本

<br></br>



