# 链接器脚本

原文 [OS Dev: Linker Script](https://wiki.osdev.org/Linker_Scripts)

---

[GNU ld](https://wiki.osdev.org/LD) 通过链接器脚本的使用，支持进行链接的进程的配置。链接器脚本使用 GNU ld 专用的脚本语言来编写。ld 并不严格规定使用链接器脚本文件，它提供了一种更易于维护的替代方法，可以将复杂的配置选项指定为命令行参数。链接器脚本最重要的用法是指定最终那个可执行二进制文件的格式和内存布局。这对 *OSDEVer* 来说很有用，因为可执行二进制文件通常需要特定的文件布局，以便某些 [bootloader](https://wiki.osdev.org/Bootloader) 辨别。[GNU GRUB](https://wiki.osdev.org/GRUB) 是其中一种 bootloader

链接器脚本通常使用 `-T script.ld` 被 ld 命令调用。

## 关键字

下面列举的都是使用链接器脚本中重要的关键字

### ENTRY

```lds
ENTRY(main)
ENTRY(MultibootEntry)
```

`ENTRY` 用来定义程序的入口点，入口点是输入文件的第一条指令。这个关键字接收链接程序/内核的入口点的符号名作为单个参数。由符号名指向的代码将会是 [ELF](https://wiki.osdev.org/ELF) 和 [PE](https://wiki.osdev.org/PE) 二进制文件 `.text` 段的第一个字节

<br></br>

### OUTPUT_FORMAT

```lds
OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_FORMAT("pe-i386")
```

`OUTPUT_FORMAT` 指令需要一个参数，指定了可执行文件的输出格式。为了找到你系统的 `binutils` 和 `GCC` 到底支持什么输出格式，可以用 `objdump -i` 查看

下面列出一些常用格式：

|格式|描述|
|--|--|
|`binary`|完全没有任何格式的平坦二进制文件|
|`elf32-i386`|`i386` 架构的 32 位 `ELF` 格式|
|`elf64-x86-64`|`x86-64` 架构的 64 位 `ELF` 格式|
|`pe-i386`|`i386` 架构的 32 位 `PE` 格式|

<br></br>

### STARTUP

```lds
STARTUP(Boot.o)
STARTUP(crt0.o)
```

`STARTUP` 需要一个参数，表示这个文件你想要链接到可执行文件的最开始处。对于用户态的程序，这通常是 `crt0.o` 或者 `crtbegin.o`；对于内核来说，这通常是一个文件，包含一些用来初始化栈、初始化 `GDT` 然后跳入内核如 `kmain()` 等的汇编代码

<br></br>

### SEARCH_DIR

```lds
SEARCH_DIR(Directory)
```

`SEARCH_DIR` 会增加一个路径到动态库搜索目录上，`-nostdlib` 会导致在该路径上找到的任何库都被忽略。我不知道为什么，但这似乎是 ld 的工作原理，通过这种方式将链接器脚本指定的搜索目录视为标准目录，因此可以使用 `-no-default-libs` 和类似标志来忽略标准目录，从而使用链接器脚本的目录

<br></br>

### INPUT

```lds
INPUT(File1.o File2.o File3.o ...)
INPUT
(
	File1.o
	File2.o
	File3.o
	...
)
```

`INPUT` 是用于将目标文件添加到命令行的 "链接器脚本" 的替代品。你通常会指定类似于 `ld <FILE1.o> <FILE2.o>` 的指令，而 `INPUT` 可用于在链接器脚本中执行此操作

<br></br>

### OUTPUT

```lds
OUTPUT(Kernel.bin)
```

`OUTPUT` 命令指定进行链接的进程要生成的文件，这是最终的二进制文件的名字。与命令行 `-o filename` 的效果相同，相当于重写

<br></br>

### MEMORY

```lds
MEMORY
{
    ROM (rx) : ORIGIN = 0, LENGTH = 256k
    RAM (wx) : org = 0x00100000, len = 1M
}
```

`MEMORY` 声明一个或更多的内存区，其属性指定该区域是否可以写入、读取或执行。通常用在嵌入式系统，因为地址空间的不同区域包含不同的访问权限

上面的示例脚本告诉链接器有两个内存区

a) "ROM" 起始地址为 `0x00000000`，长度 `256kB`，可读可执行
b) "RAM" 起始地址为 `0x00100000`，长度 `1MB`，可读可写可执行

<br></br>

### SECTIONS

```lds
SECTIONS
{
  .text.start (_KERNEL_BASE_) : {
    startup.o( .text )
  }

  .text : ALIGN(0x1000) {
_TEXT_START_ = .;
    *(.text)
_TEXT_END_ = .;
  }

  .data : ALIGN(0x1000) {
_DATA_START_ = .;
    *(.data)
_DATA_END_ = .;
  }

  .bss : ALIGN(0x1000) {
_BSS_START_ = .;
    *(.bss)
_BSS_END_ = .;
  }
}
```

这个脚本告诉链接器将 `.text` 段放在 `startup.o` 开头，从逻辑地址 `_KERNEL_BASE_` 开始。后面的页面对齐作用于所有输入文件的 `.text`、`.data` 和 `.bss` 段

链接器符号定义了保存每个段的开始地址和结束地址。这些符号在程序里有外部链接属性，可在代码中作为指针来访问。链接器文件符号的更多信息详见下文

<br></br>

### KEEP

`KEEP` 语句让链接器保留指定的段，即使其中没有引用任何符号。该语句需要在 `SECTIONS` 里面使用。当在链接阶段执行 GC 时该语句会很有用，通过传递 `--gc-sections` 来开启。`KEEP` 语句指示链接器在创建 `dependency graph` 时使用指定的结点作为根结点，以找到未使用的 `section`。本质上是强制这些 `section` 标记为已使用

该语句通常在 [ARM](https://wiki.osdev.org/Category:ARM) 架构的链接器脚本中看到，用来设置中断向量表起始于 `0x00000000`。没有这条指令，则不在代码里显示引用的中断向量表会被删除

```lds
SECTIONS
{
	.text :
	{
		KEEP(*(.text.ivt))
		*(.text.boot)
		*(.text*)
	} > ROM

	/** ... **/
}
```

<br></br>

## 符号

可以在链接器脚本中定义独立的符号。这些符号会被加入到程序的符号表。表里每个符号都有一个名字，并关联到一个地址上。在链接器脚本里已经分配到值的符号将可用于外部链接，也可以在代码中作为指针来访问

下面这个示例脚本展示符号赋值可用于三个不同地方

```lds
floating_point = 0;
SECTIONS
{
  .text :
    {
      *(.text)
      _etext = .;
    }
  _bdata = (. + 3) & ~ 3;
  .data : { *(.data) }
}
```

上面的例子中，符号 `floating_point` 被定义为 0；`_etext` 定义为一个在 `.text` 输入段最后的地址；`_bdate` 定义为一个在 `.text` 输出段之后，向上对齐 4 字节

下面是一个在 C 里使用这些符号的示例

```c
/** 链接符号在外部 */
extern uintptr_t _etext;
// ...
/** 链接符号指向二进制数据 */
uint32_t* item = &_etext;
```

<br></br>

## 更多

外链

- [GNU ld 链接器脚本部分的手册](https://sourceware.org/binutils/docs/ld/Scripts.html#Scripts)
- [GNU ld 手册（完整版）](https://sourceware.org/binutils/docs/ld/)
- [GNU ld 链接器脚本示例](http://www.bravegnu.org/gnu-eprog/lds.html)

<br></br>

# GNU Linker Scripts

原文 [GNU Linker Scripts](https://sourceware.org/binutils/docs/ld/Scripts.html)

每次链接都由链接器脚本控制，脚本用链接器命令语言编写

链接器脚本的主要目的是描述输入文件里的 `section` 应该如何映射至输出文件，并且控制输出文件的内存映射。大多数链接器脚本只会做这些事，但也会在需要的时候也会让链接器用下面描述的命令来执行其他操作

链接器总是使用一个链接器脚本。如果你不提供，使用的是链接器可执行文件内部的默认脚本。你可以用 `--verbose` 命令行选项来显示默认的链接器脚本。某些命令行选项，比如 `-r` 或 `-N` 会影响默认的链接器脚本

你可以通过 `-T` 选项来提供你自己的脚本。当你这么做的时候，你的链接器脚本会取代原来的默认脚本

你也可以通过将链接器脚本命名为链接器的输入文件来隐式使用链接器脚本，就好像它们是要链接的文件一样，详见 [隐式的链接器脚本](https://sourceware.org/binutils/docs/ld/Implicit-Linker-Scripts.html)

- [基础的脚本概念](https://sourceware.org/binutils/docs/ld/Basic-Script-Concepts.html)：基础的链接器脚本概念
- [脚本格式](https://sourceware.org/binutils/docs/ld/Script-Format.html)：链接器脚本格式
- [简单的示例](https://sourceware.org/binutils/docs/ld/Simple-Example.html)：简单的链接器脚本示例
- [简单的命令](https://sourceware.org/binutils/docs/ld/Simple-Commands.html)：简单的链接器脚本命令
- [赋值](https://sourceware.org/binutils/docs/ld/Assignments.html)：为符号赋值
- [SECTIONS](https://sourceware.org/binutils/docs/ld/SECTIONS.html)：`SECTIONS` 命令
- [MEMORY](https://sourceware.org/binutils/docs/ld/MEMORY.html)：`MEMORY` 命令
- [PHDRS](https://sourceware.org/binutils/docs/ld/PHDRS.html)：`PHDRS` 命令
- [VERSION](https://sourceware.org/binutils/docs/ld/VERSION.html)：`VERSION` 命令
- [表达式](https://sourceware.org/binutils/docs/ld/Expressions.html)：链接器脚本里的表达式
- [隐式的链接器脚本](https://sourceware.org/binutils/docs/ld/Implicit-Linker-Scripts.html)：隐式的链接器脚本

---

## 基础的链接器脚本概念

我们需要定义一些基础的概念和词汇为了描述我们的链接器脚本语言

链接器将所有输入文件合并为一个输出文件。而输入和输出文件都是 *目标文件* 格式，每个文件称为 *目标文件*。输出文件通常被称为 *可执行文件*，但这里我们仍称为 *目标文件*。每个目标文件都有一系列 `section`。我们有时候会将输入文件的 `section` 称为 *输入 `section`*，同样地，输出文件的称为 *输出 `section`*

一个目标文件里的每个 `section` 都有一个名称和长度。大多数 `section` 都有一个关联数据的块，称为 *`section` 上下文*。一个 `section` 可能会被标记为 *可加载的*（`loadable`），意味着当输出文件运行时这部分上下文应该被加载到内存中；一个没有上下文的 `section` 可能会被标记为 *可分配的*（`allocatable`），意味着应该留出内存中的一个区域，但不应该在那里加载任何其他特定的数据（在某些情况下必须将这个内存区清零）；一个既不是 *可加载* 又不是 *可分配* 的 `section` 通常包含一些调试信息

每个 *可加载* 或 *可分配* 的输出 `section` 都有两个地址。第一个是 `VMA`——虚拟内存地址，这是输出文件运行时 `section` 的地址；第二个是 `LMA`——加载内存地址，`section` 会加载到这个地址上。大多数情况下这两个地址是相同的。不同的情况是数据段被加载到 `ROM` 中，但程序启动时拷贝到 `RAM`（这种技术通常用于在基于 `ROM` 的系统中初始化全局变量）。此时 `ROM` 地址是 `LMA` 而 `RAM` 地址是 `VMA`

你可以通过 `objdump -h` 选项在一个目标文件上找到 `section`

> **译者注**
> ```shell
> [root@xxx]# objdump -h hello.o
> 
> hello.o:     file format elf64-x86-64
>
> Sections:
> Idx Name          Size      VMA               LMA               File off  Algn
>   0 .text         0000000f  0000000000000000  0000000000000000  00000040  2**0
>                   CONTENTS, ALLOC, LOAD, READONLY, CODE
>   1 .data         00000000  0000000000000000  0000000000000000  0000004f  2**0
>                   CONTENTS, ALLOC, LOAD, DATA
>   2 .bss          00000000  0000000000000000  0000000000000000  0000004f  2**0
>                   ALLOC
>   3 .comment      00000062  0000000000000000  0000000000000000  0000004f  2**0
>                   CONTENTS, READONLY
>   4 .note.GNU-stack 00000000  0000000000000000  0000000000000000  000000b1  2**0
>                   CONTENTS, READONLY
>   5 .note.gnu.property 00000020  0000000000000000  0000000000000000  000000b8  2**3
>                   CONTENTS, ALLOC, LOAD, READONLY, DATA
>   6 .eh_frame     00000038  0000000000000000  0000000000000000  000000d8  2**3
>                   CONTENTS, ALLOC, LOAD, RELOC, READONLY, DATA

每个目标文件也拥有一系列 *符号*，称为 *符号列表*。一个符号可能是 `defined` 或 `undefined`。每个符号都有一个名字、一个地址以及其他信息。如果你将一个 `C/C++` 程序编译为目标文件，你将会得到每个函数的 `defined` 符号、全局变量和静态变量。输入文件中引用的未定义函数或全局变量都会定义成 `undefined`

你可以用 `nm` 在目标文件上看到符号，或者 `objdump -t` 也可以

> **译者注**
> [root@xxx]# nm hello.o
> 0000000000000000 T main
>
> [root@xxx]# objdump -t hello.o
> 
> hello.o:     file format elf64-x86-64
>
> SYMBOL TABLE:
> 0000000000000000 l    df *ABS*  0000000000000000 hello.c
> 0000000000000000 l    d  .text  0000000000000000 .text
> 0000000000000000 g     F .text  000000000000000f main

<br></br>

## 链接器脚本格式

链接器脚本是文本文件

您将链接器脚本编写为一系列命令。每条命令要么是一个可能带参数的关键字，要么是一个符号的赋值。你可以使用分号分隔命令。空白字符通常会被忽略

通常可以直接输入文件或格式名称等字符串。如果文件名本身包含逗号等的字符，你要将文件名放在双引号中，否则会被链接器用来分割文件名

你可以在链接器脚本上包含注释，就像 C 一样，由 `/*` 和 `*/` 分割。在 C 中，注释在语法上等同于空格

<br></br>

## 简单的示例

链接器脚本大多都相当简单

最简单的只有一条命令 `SECTIONS`，去描述输出文件的内存布局

`SECTIONS` 命令是一条相当强大的命令，这里我们会描述它的简单用法。假设你的程序只包括代码和数据，其中数据既有初始化的也有未初始化的。对应的 `section` 分别是 `.text`、`.data` 和 `.bss`。进一步假设在你的输入文件中只有这些 `section`

对于这个例子，我们希望代码被加载到 `0x1_0000`，而数据起始于 `0x800_0000`，则下面的脚本可以实现这个目的：

```lds
SECTIONS
{
  . = 0x10000;
  .text : { *(.text) }
  . = 0x8000000;
  .data : { *(.data) }
  .bss : { *(.bss) }
}
```

这里使用 `SECTIONS` 关键字，后面用方括号括起来的，是一些符号的赋值以及输出 `section` 的描述

第一行的 `. = 0x10000` 给一个特殊的符号 `.` 赋值，这个符号表示的是位置计数器。如果你不以其他方式指定输出端的地址（其他方式后文介绍），则从位置计数器当前的值处开始设置地址，之后位置计数器会加上这个输出 `section` 的大小。在 `SECTIONS` 命令开头处，位置计数器的值是 0

第二行的 `.text : { *(.text) }` 定义了输出 `section` 的 `.text` 段，冒号是语法要求但现在先忽略。后面的方括号列出了应放置在此输出 `section` 内的输入 `section` 的名称。`*` 是通配符表示匹配任何文件名。`*(.text)` 意味着所有输入文件里的所有 `.text`

因为 `.text` 定义时位置计数器的值是 `0x10000`，链接器会设置输出文件的 `.text` 段地址为 `0x10000`

剩余的行定义了输出文件的 `.data` 和 `.bss` 段。链接器会将 `.data` 输出 `section` 地址设置为 `0x800_0000`。之后位置计数器会加上 `.data` 的大小，这使得 `.bss` 输出 `section` 在内存上是紧随 `.data` 之后

链接器会确保每个输出 `section` 在需要时通过增加位置计数器来对齐。在这个例子中，`.text` 和 `.data` 的地址处可以满足任何对齐限制，但链接器可能会在 `.data` 和 `.bss` 之间创建一个小间隙来对齐

这就是全部内容了，一个简单得又很完整得链接器脚本

<br></br>

## 简单的链接器脚本命令

在这部分我们会描述简单的命令

---

### 设置入口点

在一个程序里要执行的第一条指令被称为入口点。你可以使用 `ENTRY` 命令去设置入口点，参数是一个符号名：

```lds
ENTRY(symbol)
```

有好几种方式可以设置入口点。链接器通过顺序尝试以下的方法来设置入口点，如果其中一种方式成功则停止：

- `-e` 命令行选项
- 链接器脚本里的 `ENTRY(symbol)`
- 特定目标平台符号的值，对于大多数目标平台来说是 `start`，而 `PE-` 和 `BeOS-` 系统会检查一系列的入口点符号，匹配其中找到的第一个
- 如果代码段存在或已创建了可执行文件，则入口点为第一个字节的地址（代码段通常是 `.text` 但也可以是其他内容）
- 地址 0

<br></br>

### 处理文件的命令

有多个脚本命令可以处理文件：

+ ```lds
  INCLUDE filename
  ```
    表示包含的链接器脚本文件名。会在当前目录查找这个文件，也可以指定 `-L` 选项在其他目录查找。最多可以嵌套调用 `INCLUDE` 10 次  
    你可以在最高层、`MEMORY` 或 `SECTIONS` 命令、以及输出 `section` 中设置 `INCLUDE` 指令
+ ```lds
  INPUT(file, file, ...)
  INPUT(file file ...)
  ```
    `INPUT` 命令让链接器在链接阶段包含给定文件，就像就好像它们在命令行上给出的一样  
    例如，如果你总是想在任何你想链接的时候包含 `subr.o`，但你觉得每次链接都要在命令行给出这个文件很麻烦，你就可以将 `INPUT(subr.o)` 加入脚本  
    事实上，只要你想，你都可以在脚本列出所有输入文件，然后调用链接器链接时只需写上 `-T` 选项就好了  
    如果配置了 *`sysroot prefix`*，且文件名以 `'/'` 字符开头，并且当前正在执行的脚本位于 *`sysroot prefix`* 内，则将在 *`sysroot prefix`* 中查找文件名。*`sysroot prefix`* 也可以通过将 `=` 指定为文件名路径中的第一个字符或在文件名路径前加上 `$SYSROOT` 来强制使用。详见 [命令行选项](https://sourceware.org/binutils/docs/ld/Options.html) `-L` 选项  
    如果 *`sysroot prefix`* 没有使用，那么链接器会尝试打开包含链接器脚本的那个目录的文件。如果没有找到，那么链接器会在当前目录查找。如果仍没有找到，那么链接器会查找动态库的搜索路径  
    如果你使用了 `INPUT(-lfile)`，`ld` 会像命令行选项 `-l` 那样转化为 `libfile.a`  
    当你在默认链接器脚本中使用 `INPUT` 命令时，文件将包含在链接器脚本包含的位置处，这会影响动态库的搜索
+ ```lds
  GROUP(file, file, …)
  GROUP(file file …)
  ```
    和 `INPUT` 类似，除了包含文件全部都应该是动态库，并且这些文件链接器都会重复搜索，直至没有创建新的未定义引用。详见 [命令行选项](https://sourceware.org/binutils/docs/ld/Options.html) `-(` 选项
+ ```lds
  AS_NEEDED(file, file, …)
  AS_NEEDED(file file …)
  ```
    这个结构只会在 `INPUT` 命令、`GROUP` 命令或其他文件名内才会出现。列举的文件将会被处理，就像它们直接出现在 `INPUT` 命令或 `GROUP` 命令一样，除了 `ELF` 共享库，只有在实际需要时才会添加它们。这个结构本质上是为所有列举文件打开了 `--as-needed` 选项，并恢复之前的 `--as-needed`，之后设置 `--no-as-needed`
+ ```lds
  OUTPUT(filename)
  ```
    命名输出文件。在链接器脚本里使用 `OUTPUT(filename)` 完全与使用 `-o filename` 命令行选项一样（详见  [命令行选项](https://sourceware.org/binutils/docs/ld/Options.html)）。如果都使用，会首先使用命令行选项  
    你可以用 `OUTPUT` 命令为输出文件定义一个默认名，而不是通常的 `a.out`
+ ```lds
  SEARCH_DIR(path)
  ```
    `SEARCH_DIR` 命令将 `path` 增加到 `ld` 搜索动态库的路径列表中。使用 `SEARCH_DIR(path)` 完全和使用命令行选项 `-L path` 一样（详见  [命令行选项](https://sourceware.org/binutils/docs/ld/Options.html)）。如果都使用，链接器都会搜索，只是先搜索命令行选项指定的路径
+ ```lds
  STARTUP(filename)
  ```
    `STARTUP` 命令跟 `INPUT` 命令差不多，除了 `filename` 会作为第一个输入文件来链接，就像它在命令行上被第一个指定。当使用一个入口点总是在第一行开始处的系统时特别有用

<br></br>

### 处理目标文件格式的命令

用来处理目标文件格式的一组链接器脚本命令

- ```lds
  OUTPUT_FORMAT(bfdname)
  OUTPUT_FORMAT(default, big, little)
  ```
    `OUTPUT_FORMAT` 命令用于输出文件的 `BFD` 格式的命名（详见 [BFD](https://sourceware.org/binutils/docs/ld/BFD.html)）。使用 `OUTPUT_FORMAT(bfdname)` 与使用命令行选项 `--oformat bfdname` 完全相同（详见  [命令行选项](https://sourceware.org/binutils/docs/ld/Options.html)）。如果都使用，命令行选项会首先使用  
    你可以用三个参数的 `OUTPUT_FORMAT` 指定使用基于命令行选项 `-EB` 和 `-EL` 的不同格式。这允许链接器脚本设置想要的大端格式的输出文件  
    如果 `-EB` 和 `-EL` 都不使用，输出格式会被指定为第一个参数；如果只使用 `-EB`，输出格式会是第二个参数即大端；如果只使用 `-EL`，输出格式会是第三个参数即小端  
    例如，默认的 `MIPS ELF` 平台的默认链接器脚本使用这条命令：  
    ```lds
    OUTPUT_FORMAT(elf32-bigmips, elf32-bigmips, elf32-littlemips)
    ```  
    表示输出文件的默认格式是 `elf32-bigmips`，但如果用户使用了 `-EL` 选项，则输出文件会是 `elf32-littlemips` 格式
- ```lds
  TARGET(bfdname)
  ```
    当读取输入文件时 `TARGET` 命令命名要使用的 `BFD` 格式。这影响 `INPUT` 和 `GROUP` 命令的顺序。这条命令和使用命令行选项 `-b bfdname` 相似（详见  [命令行选项](https://sourceware.org/binutils/docs/ld/Options.html)）。如果使用了 `TARGET` 命令而没用 `OUTPUT_FORMAT`，则最后的 `TARGET` 命令也会用来设置输出文件的格式，详见 [BFD](https://sourceware.org/binutils/docs/ld/BFD.html)

<br></br>

### 为内存区起别名

能够用 `MEMORY` 命令为已创建的内存区增加别名。每个名字对应至多一个内存区

```lds
REGION_ALIAS(alias, region)
```

`REGION_ALIAS(alias, region)` 函数为 `region` 这个内存区创建了一个 `alias` 的别名，这允许更灵活地将输出 `section` 映射到内存区。下面是一个例子：

试想我们有一个嵌入式系统的应用，该系统用于许多内存储存种类。它们都有一个易失性内存 `RAM`，允许代码执行或数据存储；有些可能只有只读、非易失性的 `ROM`，只允许代码执行和只读数据的访问；最后一种也是只读的非易失性，但是另一种变体，`ROM2`，具有只读数据的访问而没有代码执行的能力。则此时我们有以下 4 种输出 `section`：

- `.text` 程序代码
- `.rodata` 只读数据
- `.data` 可读可写的已初始化数据
- `.bss` 可读可写的初始化为零的数据

我们的目标是提供一个链接器脚本文件，该文件包含定义输出 `section` 和系统独立部分，以及将输出 `section` 映射到系统上可用内存区域的部分。我们将这个带 3 个不同内存区的嵌入式系统设置为 `A`、`B` 和 `C`：

|section|变种 A|变种 B|变种 C|
|--|--|--|--|
|`.text`|`RAM`|`ROM`|`ROM`|
|`.rodata`|`RAM`|`ROM`|`ROM2`|
|`.data`|`RAM`|`RAM/ROM`|`RAM/ROM2`|
|`.bss`|`RAM`|`RAM`|`RAM`|

`RAM/ROM` 和 `RAM/ROM2` 意味着这个 `section` 分别加载到 `ROM` 或 `ROM2`。注意 `.data` 的加载地址在所有三种变体种都起始于 `.rodata` 末尾

下面是处理输出 `section` 的基础脚本，包含系统依赖的 `linkcmds.memory` 文件，用以描述内存布局：

```lds
INCLUDE linkcmds.memory

SECTIONS
{
  .text :
    {
      *(.text)
    } > REGION_TEXT
  .rodata :
    {
      *(.rodata)
      rodata_end = .;
    } > REGION_RODATA
  .data : AT (rodata_end)
    {
      data_start = .;
      *(.data)
    } > REGION_DATA
  data_size = SIZEOF(.data);
  data_load_start = LOADADDR(.data);
  .bss :
    {
      *(.bss)
    } > REGION_BSS
}
```

现在我们需要三个不同的 `linkcmds.memory` 文件来定义你内存区和别名。下面是这三个变体 `A`、`B` 和 `C` 的 `linkcmds.memory` 文本内容：

- `A`
  这里所有段都是 `RAM`  
  ```lds
  MEMORY
  {
    RAM : ORIGIN = 0, LENGTH = 4M
  }

  REGION_ALIAS("REGION_TEXT", RAM);
  REGION_ALIAS("REGION_RODATA", RAM);
  REGION_ALIAS("REGION_DATA", RAM);
  REGION_ALIAS("REGION_BSS", RAM);
  ```
- `B`
  程序代码和只读数据设置为 `ROM`，可读可写数据设置为 `RAM`。已初始化数据的镜像一开始加载到 `ROM`，系统运行伊始就拷贝到 `RAM`  
  ```lds
  MEMORY
  {
    ROM : ORIGIN = 0, LENGTH = 3M
    RAM : ORIGIN = 0x10000000, LENGTH = 1M
  }

  REGION_ALIAS("REGION_TEXT", ROM);
  REGION_ALIAS("REGION_RODATA", ROM);
  REGION_ALIAS("REGION_DATA", RAM);
  REGION_ALIAS("REGION_BSS", RAM);
  ```
- `C`
  程序代码设置为 `ROM`，只读数据为 `ROM2`，可读可写数据为 `RAM`。已初始化数据的镜像一开始加载到 `ROM2`，系统运行伊始就拷贝到 `RAM`  
  ```lds
  MEMORY
  {
    ROM : ORIGIN = 0, LENGTH = 2M
    ROM2 : ORIGIN = 0x10000000, LENGTH = 1M
    RAM : ORIGIN = 0x20000000, LENGTH = 1M
  }

  REGION_ALIAS("REGION_TEXT", ROM);
  REGION_ALIAS("REGION_RODATA", ROM2);
  REGION_ALIAS("REGION_DATA", RAM);
  REGION_ALIAS("REGION_BSS", RAM);
  ```

如果需要的话可以写一个通用的系统初始化例程，用以将 `.data` 从 `ROM` 或 `ROM2` 拷贝到 `RAM`：

```c
#include <string.h>

extern char data_start [];
extern char data_size [];
extern char data_load_start [];

void copy_data(void)
{
  if (data_start != data_load_start)
    {
      memcpy(data_start, data_load_start, (size_t) data_size);
    }
}
```

<br></br>

### 其他链接器脚本命令

这里还有一些其他的链接器脚本命令

- `ASSERT(exp, message)`
  确保 `exp` 非零。如果为零，退出链接器并返回一个错误码以及打印 `message`  
  注意是在链接的最后阶段发生之前才会去检查断言。这意味着如果用户没有为这些符号设置值，则该符号相关的表达式将失败。唯一的例外是仅引用点的符号，因此一个断言看起来像这样：  
  ```lds
  .stack :
  {
    PROVIDE (__stack = .);
    PROVIDE (__stack_size = 0x100);
    ASSERT ((__stack > (_end + __stack_size)), "Error: No room left for the stack");
  }
  ```
  如果到处都找不到 `__stack_size` 的定义就失败。  
  在 `section` 外定义的符号会被更早地检索到，因此它们可用于在断言种，即：  
  ```lds
  PROVIDE (__stack_size = 0x100);
  .stack :
  {
    PROVIDE (__stack = .);
    ASSERT ((__stack > (_end + __stack_size)), "Error: No room left for the stack");
  }
  ```
  就可以工作
- `EXTERN(symbol symbol …)`
  强制将要进入输出文件的 `symbol` 变成一个未定义符号。例如，这样做可能会触发标准库中附加模块的链接。你可以在每个 `EXTERN` 命令种列举多个 `symbol`，也可以多次使用 `EXTERN`。这个命令与命令行选项 `-u` 完全相同
- `FORCE_COMMON_ALLOCATION`
  这个命令与命令行选项 `-d` 完全相同，用来使 `ld` 为普通符号分配空格，就像一个可重定向的输出文件被指定了 `-r`
- `INHIBIT_COMMON_ALLOCATION`
  这个命令与命令行选项 `--no-define-common` 完全相同，用来使 `ld` 删除普通符号地址上的赋值，即使这是一个非可重定向的输出文件
- `FORCE_GROUP_ALLOCATION`
  这个命令与命令行选项 `--force-group-allocation` 完全相同，用来使 `ld` 像普通输入 `section` 那样设置 `section` 组，然后删除`section` 组，即使指定了可重定向的输出文件
- `INSERT [ AFTER | BEFORE ] output_section`
  此命令通常在由 `-T` 命令行选项使用，以增加默认的 `SECTIONS`，例如覆盖。它在 `output_section` 之后（或之前）插入所有先前的连接器脚本语句，之后会导致 `-T` 选项不重写默认的链接器脚本。确切的插入点与 `orphan section` 相同，详见 [位置计数器](https://sourceware.org/binutils/docs/ld/Location-Counter.html)。插入发生在链接器将输入 `section` 映射至输出 `section` 完成之后。在插入之前，由于 `-T` 默认在链接器脚本之前解析，因此 `-T` 中的语句出现在脚本的内部链接器中的默认链接器脚本语句之前。特别地，输入`section` 赋值语句将会在 `-T` 输出 `section` 赋值语句之前。这里是一个 `-T` 使用 `INSERT` 怎么工作的例子：
  ```lds
  SECTIONS
  {
    OVERLAY :
    {
      .ov1 { ov1*(.text) }
      .ov2 { ov2*(.text) }
    }
  }
  INSERT AFTER .text;
  ```
- `NOCROSSREFS(section section …)`
  这个命令可用来告诉 `ld` 提出一个在某输出 `section` 中关于任何引用符号的错误  
  在某些类型的程序中，特别是在嵌入式系统中用到覆盖的程序时，当一个 `section` 加载到内存时，就不会再加载另一个 `section` 了。在两个 `section` 之间任何直接的引用符号都会出错。比如，如果一个 `section` 的代码调用其他 `section` 的函数就会出错  
  `NOCROSSREFS` 命令参数是一系列输出 `section` 名字。如果 `ld` 在这些段之间检测到任何交叉引用，就会提出错误，然后返回一个非零退出状态。注意 `NOCROSSREFS` 命令使用的是输出 `section` 名而不是输入名  
- `NOCROSSREFS_TO(tosection fromsection …)`
  此命令可用于告诉 `ld` 提出错误关于一个 `section` 中的任何引用，这个 `section` 来自一系列其他 `section`  
  `NOCROSSREFS` 命令用来确保两个或更多输出 `section` 是完全独立的，但某些情况下需要单向依赖。比如，在一个多核应用中有一些共享代码是从其他核心处调用的，但出于安全考虑不允许回调  
  `NOCROSSREFS_TO` 命令参数也是一系列输出 `section` 名字。第一个 `section` 不能被其他任何 `section` 中引用。如果 `ld` 检测到任何第一个 `section` 的引用来自其他 `section`，则报告一个错误，然后返回一个非零退出状态。注意 `NOCROSSREFS_TO` 命令使用的是输出 `section` 名而不是输入名
- `OUTPUT_ARCH(bfdarch)`
  指定一个特定的输出机器的架构，参数是 `BFD` 库（详见 [BFD](https://sourceware.org/binutils/docs/ld/BFD.html) ）使用名字的其中一个。你可以通过 `objdump -f` 选项看到一个目标文件的架构
- `LD_FEATURE(string)`
  这个命令用来更改 `ld` 行为。如果 *`string`* 是 "SANE_EXPR" 则脚本里的绝对符号和数字在任何地方都会被简单地视为数字。详见 [表达式 `Section`](https://sourceware.org/binutils/docs/ld/Expression-Section.html)

<br></br>

## 向符号赋值

在链接器脚本中你可以向符号赋值，这即为定义一个符号，并且该值会加入全局符号表

---

### 简单赋值

你可以使用任何 C 的赋值运算符来赋值一个符号

```c
symbol = expression ;
symbol += expression ;
symbol -= expression ;
symbol *= expression ;
symbol /= expression ;
symbol <<= expression ;
symbol >>= expression ;
symbol &= expression ;
symbol |= expression ;
```

第一种行会定义符号为 `expression` 的值，其他行 `symbol` 必须要先定义，然后才会相应调整表达式的值

一个特殊的符号，`.`，指示着位置计数器，你可以在 `SECTIONS` 命令中只使用这个符号，详见 [位置计数器](https://sourceware.org/binutils/docs/ld/Location-Counter.html)

注意在 `expression` 后面的分号是必须的

下面定义的是表达式，详见 [表达式](https://sourceware.org/binutils/docs/ld/Expressions.html)

你可以将符号赋值写成命令，或 `SECTIONS` 命令里的语句，或 `SECTIONS` 命令里的输出 `section` 描述中的一部分

符号 `section` 将从表达式 `section` 设置，详见 [表达式 `Section`](https://sourceware.org/binutils/docs/ld/Expression-Section.html)

这里是一个例子在三个不同地方展示如何使用符号赋值：

```lds
floating_point = 0;
SECTIONS
{
  .text :
    {
      *(.text)
      _etext = .;
    }
  _bdata = (. + 3) & ~ 3;
  .data : { *(.data) }
}
```

在这个例子中，符号 `floating_point` 会定义为 0，`_etext` 会定义为最后一个 `.text` 输入 `section` 随后的地址，`_bdata` 会定义为 `.text` 输出 `section` 向上对齐 4 字节边界的地址

<br></br>

### 隐藏符号

对于 `ELF` 目标端口，定义一个将隐藏且不会导出的符号的语法是 `HIDDEN(symbol = expression)`

这里是一个来自 [简单赋值](https://sourceware.org/binutils/docs/ld/Simple-Assignments.html) 的例子，现在用 `HIDDEN` 重写：

```lds
HIDDEN(floating_point = 0);
SECTIONS
{
  .text :
    {
      *(.text)
      HIDDEN(_etext = .);
    }
  HIDDEN(_bdata = (. + 3) & ~ 3);
  .data : { *(.data) }
}
```

此时该模块中三个符合没有一个可以被外部程序使用

<br></br>

### PROVIDE

在某些情况下，我们希望链接器脚本仅在符号被引用且未由其他任何对象定义时才定义符号。比如，传统的链接器定义 `.etext`。然而，`ANSI C` 要求用户能够使用 `.etext` 作为一个函数名而不会遇到错误。`PROVIDE` 关键字用在它被引用且无定义时，才定义一个符号，如 `.etext`。语法是 `PROVIDE(symbol = expression)`

这里是一个用 `PROVIDE` 定义 `.etext` 的例子：

```lds
SECTIONS
{
  .text :
    {
      *(.text)
      _etext = .;
      PROVIDE(etext = .);
    }
}
```

这个例子中，如果程序定义了 `_etext`（带前缀下划线），则链接器会给出重定义提示。另一方面，如果程序定义了 `etext`（没有前缀下划线），则链接器才使用程序的定义。如果程序引用 `etext` 但没有定义，则链接器会使用链接器脚本里的定义

*注意* - `PROVIDE` 指令将一个常用符号视为将要定义，即使这样一个符号能够用 `PROVIDE` 创建的符号合并。当考虑构造和析构列表符号时特别重要，比如 `__CTOR_LIST__` 这些符号通常被定义为常用符号

<br></br>

### PROVIDE_HIDDEN

和 `PROVIDE` 相似，对于 `ELF` 目标端口，符号将会被隐藏并且不会导出

<br></br>

### 源码引用符号

从源码中访问链接器脚本定义的变量并不直观，特别是链接器脚本里的符号和高级语言中的变量定义并不等同，实际上该符号是并没有值

在继续之前，要明白很重要的一点是，编译器总是将源码里的变量名转换为符号表里的不同名字。比如，`Fortran` 编译器通常优先考虑或主动添加一个下划线，`C++` 执行广泛的 `name mangling`。因此可能在源码中变量使用的名字和定义在链接器脚本中的同一个变量的名字有差异。比如在 C 语言里链接器脚本变量像下面这样引用：

```c
extern int foo;
```

但在链接器脚本中是这样定义的

```lds
_foo = 1000;
```

不过现在假设接下来我们讨论的例子中不出现变量名转换

当在高级语言比如 C 里声明一个符号实际上发生了两件事。第一是编译器保留了足够的内存空间以保存符号的值；第二是编译器在符号表里创建了一个条目，这个表保存了符号的地址，即保存的是这个符号对应那个内存块的地址。所以对于下面这个例子：

```c
int foo = 1000;
```

从文件范围来看，在符号表上创建了一个称为 `foo` 的条目，该条目对应一个 `int` 型变量的大小的内存块以初始化储存数值 `1000`

当程序引用一个编译器生成的符号时，首先访问的是符号表，从中找到符号底下那个内存块的地址，然后才从该内存块里读取数值，所以：

```c
foo = 1;
```

会在符号表里寻找符号 `foo`，获取这个符号对应的地址，然后将 `1` 写入这个地址。但是。

```c
int *a = &foo;
```

在符号表里寻找符号 `foo`，获取其地址，然后拷贝这个地址到变量 `a` 关联的那个内存块上

相比之下，链接器脚本的符号声明在符号表里创建一个条目但不分配内存。因此它们是一个没有值的地址。下面示例给出了链接器脚本的符号定义：

```lds
foo = 1000;
```

在符号表里创建了一个叫做 `foo` 的条目，用以保存内存地址 1000，但这个地址上并没什么东西存在。这意味着你不能访问链接器脚本定义的符号的值（因为没有值），你只能访问链接器脚本定义的这个符号的地址

因此当你在源码里使用链接器脚本定义的符号时，你应该总是取地址而不要尝试使用该地址上的值。比如下面这个例子中，你想要拷贝 `.ROM` 这个内存段的内容到 `.FLASH` 段，并且链接器脚本包含这些声明：

```lds
start_of_ROM   = .ROM;
end_of_ROM     = .ROM + sizeof (.ROM);
start_of_FLASH = .FLASH;
```

那么在 C 源码里执行拷贝会是：

```c
extern char start_of_ROM, end_of_ROM, start_of_FLASH;

memcpy (&start_of_FLASH, &start_of_ROM, &end_of_ROM - &start_of_ROM);
```

注意 `&` 的使用，上面这样做才是对的。或者像下面这样，将符号视为 `vector` 或数组名也可以：

```c
extern char start_of_ROM[], end_of_ROM[], start_of_FLASH[];

memcpy (start_of_FLASH, start_of_ROM, end_of_ROM - start_of_ROM);
```

这样使用就不需要 `&` 了

<br></br>

## SECTIONS 命令

`SECTIONS` 命令告诉链接器怎样将输入 `section` 映射为输出 `section`，以及在内存中怎样安排输出 `section`

`SECTIONS` 命令格式为：

```lds
SECTIONS
{
  sections-command
  sections-command
  …
}
```

每个 `sections-command` 可以是以下情况之一：

- 一个 `ENTRY` 命令（详见 [`Entry` 命令](https://sourceware.org/binutils/docs/ld/Entry-Point.html)）
- 一个符号赋值（详见 [赋值](https://sourceware.org/binutils/docs/ld/Assignments.html)）
- 一个输出 `section` 描述
- 一个描述的覆盖

为了方便在这些命令中使用位置计数器，允许在 `SECTIONS` 命令中使用 `ENTRY`
命令和符号赋值。这也使得链接器脚本更容易理解，因为你可以在输出文件的内存布局上，在某些需要的位置上使用

下面是输出 `section` 描述和覆盖描述

如果你在链接器脚本中不使用 `SECTIONS` 命令，则链接器将按照在输入文件中第一次遇到这些 `section` 的顺序将每个输入 `section` 放入一个名称相同的输出 `section` 中。比如，如果所有输入 `section` 都出现在第一个文件中，则输出文件中 `section` 的顺序和第一个输入文件一致。第一个 `section` 起始于地址 0 处

---

### 输出段描述

输出 `section` 的完整描述看起来像这样：

```lds
section [address] [(type)] :
  [AT(lma)]
  [ALIGN(section_align) | ALIGN_WITH_INPUT]
  [SUBALIGN(subsection_align)]
  [constraint]
  {
    output-section-command
    output-section-command
    …
  } [>region] [AT>lma_region] [:phdr :phdr …] [=fillexp] [,]
```

实际上上面这些属性很多都不使用

`section` 后面的空白符是必须的，以便 `section` 名清楚明白，冒号和方括号也是必须的。如果用了 `fillexp` 并且下一个 `sections-command` 看起来像是表达式的延续，那么最后的逗号也是必须的。而换行符和其他的空白符则是可选的

每个 `output-section-command` 可能是下面列举的其中一种情况：

- 一个符号赋值（详见 [赋值](https://sourceware.org/binutils/docs/ld/Assignments.html) ）
- 一个输入 `section` 描述（详见 [输入 `Section`](https://sourceware.org/binutils/docs/ld/Input-Section.html) ）
- 要直接包含的数据值（详见 [输出 `Section` 数据](https://sourceware.org/binutils/docs/ld/Output-Section-Data.html) ）
- 一个特殊的输出 `section` 关键字（详见 [输出 `section` 关键字](https://sourceware.org/binutils/docs/ld/Output-Section-Keywords.html) ）

<br></br>

### 输出段名字

输出段的名字, *`section`*，它起名有输出格式的限制。在仅支持有限数量的 `section` 的格式中，如 `a.out`，名称必须是该格式支持的名称之一（如 `a.out` 只允许 `.text`、`.data` 或 `.bss`）。如果输出格式支持任意数量，但带有数字而不是名称（像 `Oasys` 的情况），则名称中的数字应加上引号。一个 `section` 名可能由任意字符序列组成，但一个包含任意反常字符如逗号等的名字则应加上引号

`/DISCARD/` 这个输出 `section` 名是特殊的，详见 [丢弃的输出 `Section`](https://sourceware.org/binutils/docs/ld/Output-Section-Discarding.html)

<br></br>

### 输出段地址

*`address`* 是一个用于输出 `section` `VMA`（虚拟内存地址）的表达式。这个地址是可选的，但如果提供了则将会指定一个确切的输出地址

如果输出地址没有指定则基于以下情况为输出段选择一个。该地址会调整以适应输出 `section` 的要求。对齐是输出 `section` 中包含的输入 `section` 的严格对齐

输出 `section` 地址选择顺序如下：

- 如果为这个 `section` 设置了一个输出内存区 *`region`*，那么它将被添加到该区域，其地址是该区域的下一个空闲区域
- 如果已经用了 `MEMORY` 命令来创建一系列内存区，则第一个与该 `section` 属性兼容的区域会被选中。

[3: Output Section Address](https://sourceware.org/binutils/docs/ld/SECTIONS.html)

