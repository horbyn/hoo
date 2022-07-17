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

### 输出段的描述

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

- 如果为这个 `section` 设置了一个输出内存区 *`region`*，那么它将被添加到该区域，其地址是该区域的下一个空闲地址
- 如果已经用了 `MEMORY` 命令来创建一系列内存区，则第一个与该 `section` 属性兼容的区域会被选中。段的输出地址将会是该区域内下一个空闲地址。详见 [MEMORY](https://sourceware.org/binutils/docs/ld/MEMORY.html)
- 如果没有指定内存区，或者没有匹配的段，则输出地址会基于当前位置计数器的值

举个例子：

```lds
.text . : { *(.text) }
```

和

```lds
.text : { *(.text) }
```

有一些不同。第一个将 `.text` 输出段地址设置为当前位置计数器的值。第二个设置为与位置计数器的当前值，严格对齐任何 `.text` 输入段的值

*`address`* 可以是一个单独的表达式，详见 [表达式](https://sourceware.org/binutils/docs/ld/Expressions.html)。比如，如果你想将一个段对齐 `0x10`，以便该段地址最低 4 个比特是零，那么你可以像下面这样做：

```lds
.text ALIGN(0x10) : { *(.text) }
```

这使得 `ALIGN` 返回向上对齐指定的值

为一个段指定 *`address`* 将改变位置计数器的值，前提是这个段非空（空的段会被忽略）

<br></br>

### 输入段的描述

最常用的输出端命令是输入段描述

输入段描述是最基础的链接器脚本操作。你使用输出端去告诉链接器怎样为你的程序安排内存区，使用输入段描述去告诉链接器怎样将输入文件映射为你的内存布局

---

#### 输入段基础

一个输入段描述由一个文件名组成，当然可以选择在文件名后面用括号给出一个段名列表

文件名和段名可以是通配符模式，下面会进一步描述（详见 [输入段通配符](https://sourceware.org/binutils/docs/ld/Input-Section-Wildcards.html)）

最常用的输入段描述是在输出段里，用一个特定的名称包含所有输入段。比如，为了包含所有文件的 `.text` 段，你需要这样写：

```lds
*(.text)
```

这里 `*` 就是通配符，用来匹配任何文件名。为了排除一些来自通配符的文件名，`EXCLUDE_FILE` 可用来匹配所有文件除了在它列表上指定的，比如：

```lds
EXCLUDE_FILE (*crtend.o *otherfile.o) *(.ctors)
```

这会导致除了 `crtend.o` 和 `otherfile.o`，其他所有来自文件的 `.ctors` 段都会被包含进去。`EXCLUDE_FILE` 也可以在段列表里面设置，比如：

```lds
*(EXCLUDE_FILE (*crtend.o *otherfile.o) .ctors)
```

这个例子的结果与前面的完全一致。如果段列表包含超过一个段，那么这两种 `EXCLUDE_FILE` 语法是非常有用的，比如下面这样

这里有两种方式去包含超过一个段：

```lds
*(.text .rdata)
*(.text) *(.rdata)
```

这两句脚本区别是 `.text` 和 `.rdata` 这两个输入段出现在输出段上的顺序。第一行，它们混合一起，以链接器搜索时的输入顺序出现在输出文件中；第二行，所有 `.text` 输入段出现在前面，之后紧跟的才是 `.rdata`

当在超过一个段上使用 `EXCLUDE_FILE` 时，并且这种排除行为发生在段列表上，则只发生在紧跟的段上面，举个例子：

```lds
*(EXCLUDE_FILE (*somefile.o) .text .rdata)
```

会导致除了 `somefile.o` 外其余所有 `.text` 段被包含，而 `.rdata` 段（包括里面的 `somefile.o`）也会被包含。为了排除 `.rdata` 里的 `somefile.o` 上面的例子应该改为：

```lds
*(EXCLUDE_FILE (*somefile.o) .text EXCLUDE_FILE (*somefile.o) .rdata)
```

另一种方法是，将 `EXCLUDE_FILE` 设置在段列表外面，或者说是在输入文件前面，这才会应用在所有段上，因此前面的例子可以写成：

```lds
EXCLUDE_FILE (*somefile.o) *(.text .rdata)
```

如果你的一个或更多文件的包含需要位于内存中特定位置的特殊数据，你可以指定一个文件名去包含来自特定文件的段。比如：

```lds
data.o(.data)
```

为了根据输入段的标志细化要包含的段，可以使用  `INPUT_SECTION_FLAGS`

这里是一个使用 `ELF` 段首标记（`Section header flags`）的简单例子：

```lds
SECTIONS {
  .text : { INPUT_SECTION_FLAGS (SHF_MERGE & SHF_STRINGS) *(.text) }
  .text2 :  { INPUT_SECTION_FLAGS (!SHF_WRITE) *(.text) }
}
```

这个例子中，输出段 `.text` 由一些匹配 `*(.text)` 名字的输入段组成，这些名字带 `SHF_MERGE` 和 `SHF_STRINGS` 标记。而输出段 `.text2` 匹配的名字不带 `SHF_WRITE` 标记

你也可以指定不同归档文件，语法是：先是一个匹配存档的模式，然后是一个冒号，最后是匹配文件的模式。注意冒号前后没有空白符

```lds
‘archive:file’
```

匹配相应存档的文件

```lds
‘archive:’
```

匹配全部存档

```lds
‘:file’
```

匹配不在存档中的文件

包含 `shell` 通配符的 `'archive'` 和 `'file'` ，要么一个要么两个。在基于 `DOS` 的文件系统上，链接器保证单个小写字母后跟一个冒号是这种规范，所以 `'c:myfile.o'` 是一个很简单的规范，而不会解释为名为 `'c'` 的存档里的 `'mufile.o'`。`'archive:file'` 的文件规范也可以用在 `EXCLUDE_FILE` 列表中，但可能不会出现在其他链接器脚本的内容上。比如，你不能在 `INPUT` 命令中，通过 `'archive:file'` 提取一个来自存档的文件

如果你使用一个不带段列表的文件名，则输入文件中的所有段都会被包含到输出文件中。这并不常见，但有时可能有用，比如：

```lds
data.o
```

当你使用一个既不是 `'archive:file'` 规格，也没有安和通配符字符的文件名时，链接器首先会在命令行或 `INPUT` 命令中检查你是否指定了文件名。如果没有，则尝试将文件作为输入文件打开，就好像它出现在命令行上一样。注意这和 `INPUT` 命令不同，因为链接器不会在存档搜索路径中搜索文件

<br></br>

#### 输入段通配符模式

> [原文链接](https://sourceware.org/binutils/docs/ld/Input-Section-Wildcards.html)

在一个输入端描述中，通配符可用于文件名、段名或两者上

在许多例子中见到的 `'*'` 文件名只是一个很简单的文件名通配符模式

通配符看起来和 `Unix shell` 是差不多的

`'*'`

匹配任何数量的字符

`'?'`

匹配任何单个的字符

`'[chars]'`

匹配任何字符的单个形式，`'-'` 用于指定字符范围，如 `'[a-z]'` 匹配任和小写字符

`'\'`

引用其后的字符

文件名通配符只匹配命令行或 `INPUT` 命令显式指定的文件。链接器不会搜索目录以扩展通配符

如果一个文件名匹配超过一个通配符，或者如果一个文件名显式出现，且被某个通配符匹配，那么链接器会使用链接器脚本上的第一个匹配规则。举个例子，这个输入段列表描述可能是错的，因为不会使用 `data.o` 规则：

```lds
.data : { *(.data) }
.data1 : { data.o(.data) }
```

通常，链接器会将匹配通配符的文件或段，以链接阶段被发现的顺序来设置。你可以通过 `SORT_BY_NAME` 关键字来改变这种设置，该关键字在括号里的通配符模式之前出现（如 `SORT_BY_NAME(.text*)`）。当使用了 `SORT_BY_NAME` 关键字，链接器将文件或段按名称升序排序，然后将它们放入输出文件

`SORT_BY_ALIGNMENT` 和 `SORT_BY_NAME` 差不多。`SORT_BY_ALIGNMENT` 对段进行对齐的降序排序，然后将它们放入输出文件。将大对齐放在小对齐前面可以减少填充的数量

`SORT_BY_INIT_PRIORITY` 也和 `SORT_BY_NAME` 差不多。`SORT_BY_INIT_PRIORITY` 的排序方式和段名有关，段名会按 `GCC` `init_priority` 属性进行编码，然后对段名编码的数值进行升序排序，之后才将它们放入输出文件。在 `.init_array.NNNNN` 和 `.fini_array.NNNNN` 中，`NNNNN` 就是 `init_priority` 属性。在 `.ctors.NNNNN` 和 `.dtors.NNNNN` 中, `NNNNN` 是 65535 减去 `init_priority`

`SORT` 是 `SORT_BY_NAME` 的别名

当链接器脚本中有嵌套的段排序命令时，至多只能有 1 层

1. `SORT_BY_NAME`（嵌套 `SORT_BY_ALIGNMENT`）：这首先会通过名字排序输入段，如果两个段名相同再通过对齐值排序
2. `SORT_BY_ALIGNMENT`（嵌套 `SORT_BY_NAME`）：这首先会通过对齐值排序输入段，如果两个段有相同对齐值才用文件名排序
3. `SORT_BY_NAME`（嵌套 `SORT_BY_NAME`）：与不嵌套的 `SORT_BY_NAME` 效果相同
4. `SORT_BY_ALIGNMENT`（嵌套 `SORT_BY_ALIGNMENT`）：与不嵌套的 `SORT_BY_ALIGNMENT` 效果相同
5. 所有其他嵌套段的排序均无效

当均使用了命令行段排序选项和链接器脚本段排序命令时，段排序命令总是优先于命令行选项

如果链接器脚本里的段排序命令没有嵌套，则命令行选项会被段排序命令被视为嵌套

1. 脚本的 `SORT_BY_NAME` 带 `--sort-section alignment` 选项，等价于 `SORT_BY_NAME`（嵌套 `SORT_BY_ALIGNMENT`）
2. 脚本的 `SORT_BY_ALIGNMENT` 带 `--sort-section name` 选项，等价于 `SORT_BY_ALIGNMENT`（嵌套 `SORT_BY_NAME`）

如果链接器脚本里的段排序命令是嵌套的，则命令行选项会被忽略

`SORT_NONE` 通过忽略命令行段排序选项而禁止排序

如果你仍困惑输入段在什么地方，可以用 `-M` 链接器选项生成映射文件（**译者注：`.map` 文件**）。映射文件精确地给出输入段怎样映射为输出段

这个例子给出了怎么用通配符模式对文件进行分区。脚本让链接器将所有 `.text` 段放入 `.text` 段；将所有 `.bss` 段放入 `.bss` 段；将所有以大写字母开头的文件的 `.data` 段放入 `.DATA` 段，而其余文件的 `.data` 段放入 `.data` 段

```lds
SECTIONS {
  .text : { *(.text) }
  .DATA : { [A-Z]*(.data) }
  .data : { *(.data) }
  .bss : { *(.bss) }
}
```

<br></br>

#### 常见符号的输入段

> [原文地址](https://sourceware.org/binutils/docs/ld/Input-Section-Common.html)

常见符号需要一个特殊的符号，因为再许多目标文件格式中，常见符号并没有特定的输入段。链接器将常见符号视为位于 `'COMMON'` 输入段

**You may use file names with the `'COMMON'` section just as with any other input sections.** 这可用来将特定输入文件中的常见符号放在某个段中，而将其他输入文件的常见符号放在另一个段

在大部分情况里，输入文件里的常见符号会放入输出文件的 `.bss` 段，比如：

```lds
.bss { *(.bss) *(COMMON) }
```

一些目标文件格式有超过一种常见符号类型。比如，`MIPS ELF` 目标文件格式严格区分标准常见符号和不常用常见符号。这种情况下，链接器会使用一个不同的特别的段名存放其他常见符号类型。对于 `MIPS ELF` 的情况，链接器将 `'COMMON'` 对应标准常见符号；而 `'.scommon'` 对应不常用常见符号。这允许你将不同常见符号类型映射至不同的内存区

你有时可能会再老式脚本里看到 `'[COMMON]'`，这个符号等价于 `'*(COMMON)'`，但现在视为过时

<br></br>

#### 输入段和垃圾回收

> [原文地址](https://sourceware.org/binutils/docs/ld/Input-Section-Keep.html)

当在链接阶段使用了垃圾回收（`'--gc-sections'`），标记不应删除的段时通常很有用。语法是用 `KEEP()` 括起来输入段的通配符条目，比如 `KEEP(*(.init))` 或者 `KEEP(SORT_BY_NAME(*)(.ctors))`

<br></br>

#### 输入段例子

> [原文地址](https://sourceware.org/binutils/docs/ld/Input-Section-Example.html)

接下来的例子是一个完整的链接器脚本。它告诉链接器读取所有来自 `all.o` 文件的段，并放入输出段 `'outputa'` 开头处，而 `'outputa'` 起始地址为 `'0x1_0000'`。紧接着的是来自 `foo.o` 文件的所有 `'.input1'` 段，也放入同一个输出段。所有来自 `foo.o` 的 `'input2'` 段放入输出段 `'outputb'`，后面跟着来自 `foo1.o` 的 `'.input1'`。剩余来自任何文件的所有 `'.input1'` 和 `'.input2'` 段均写入输出段 `'outputc'`

```lds
SECTIONS {
  outputa 0x10000 :
    {
    all.o
    foo.o (.input1)
    }
  outputb :
    {
    foo.o (.input2)
    foo1.o (.input1)
    }
  outputc :
    {
    *(.input1)
    *(.input2)
    }
}
```

如果一个输出段的名字和输入段的名字相同，并且可表示为 `C` 的标识符，则链接器会自动定义（详见 [PROVIDE](https://sourceware.org/binutils/docs/ld/PROVIDE.html)）两个符号：`__start_SECNAME` 和 `__stop_SECNAME`，其中，`SECNAME` 是段名。它们分别指出输出段的起始和结束地址。注意：大多段名都不可表示为 `C` 的标识符，因为有 `.` 字符

<br></br>

### 输出段的数据

> [原文地址](https://sourceware.org/binutils/docs/ld/Output-Section-Data.html)

你可以在一个输出段里通过使用 `BYTE`、`SHORT`、`LONG`、`QUAD` 或 `SQUAD` 来指定数据确切的字节数，就像使用一个输出段命令一样。每个关键字后跟一个用括号括起的表达式，表达式里需要提供要储存的值（详见 [表达式](https://sourceware.org/binutils/docs/ld/Expressions.html)）。表达式的值储存在当前位置计数器那个位置

`BYTE`、`SHORT`、`LONG` 和 `QUAD` 分别储存一个、两个、四个和八个字节。储存完后，位置计数器还会加上增加了的字节数

比如，下面这个例子会储存字节 1，紧跟着的是四字节的符号 `'addr'` 的值

```lds
BYTE(1)
LONG(addr)
```

当使用一个 64 为主机名或目标地址，`QUAD` 和 `SQUAD` 都是一样效果，都是储存八个字节（64 位）的值。而当主机名和目标地址都是 32 位时，表达式会计算为 32 位。这时 `QUAD` 储存一个由 32 位零扩展为 64 位的值，而 `SQUAD` 储存一个符号扩展的 64 位值

如果输出文件的目标文件格式具有显式字节序（通常都是这样的），则该值将储存在这种字节序下。否则，该值储存在第一个输入目标文件的字节序中，如 `S 记录`

注意，这些命令只在一个段描述里面起作用，而不是在段描述之间，所以接下来的例子会产生一个链接器的错误

```lds
SECTIONS { .text : { *(.text) } LONG(1) .data : { *(.data) } } 
```

作为对比，下面这个才是正确的

```lds
SECTIONS { .text : { *(.text) ; LONG(1) } .data : { *(.data) } } 
```

你可以位当前段使用 `FILE` 命令来设置填充，语法是后跟一个括号括起来的表达式。段内任何其他未指定的内存区（比如由于输入段的对齐而留下的间隙）都将填充未表达式的值，并按需重复。一个 `FILE` 语句覆盖了它在段里出现点后的内存区，通过包含多个 `FILE` 语句，可以在输出段的不同部分填充不同东西

下面例子展示了怎么对未指定的内存区填充为 `'0x90'`：

```lds
FILL(0x90909090)
```

`FILE` 命令和输出段属性 `'=fillexp'` 是相似的，只是它只影响紧跟 `FILE` 的那部分，而不是整个段。如果都是用，则优先使用 `FILE` 命令，详见 [输出段填充](https://sourceware.org/binutils/docs/ld/Output-Section-Fill.html) 关于填充表达式的内容

<br></br>

### 输出段关键字

> [原文地址](https://sourceware.org/binutils/docs/ld/Output-Section-Keywords.html)

有些可以作为输出段命令出现的关键字

- `CREATE_OBJECT_SYMBOLS`
  这个命令告诉链接器为每个输入文件创建一个符号，其符号名对应着输入文件。每个符号对应段将放入 `CREATE_OBJECT_SYMBOLS` 命令出现的输出段  
  对于 `a.out` 目标文件格式来说，这种安排是约定俗成的，但对于其他目标文件格式则不是
- `CONSTRUCTORS`
  使用 `a.out` 目标文件格式进行链接时，链接器使用不常见的集合结构来支持 `C++` 全局构造函数和析构函数。是链接一些不支持任意段的目标文件格式时（比如 `ECOFF` 和 `XCOFF`），链接器自动根据名字来区分 `C++` 全局构造函数和全局析构函数。对于这些目标文件格式来说，`CONSTRUCTORS` 命令用来告诉链接器将构造函数信息放入该命令出现处。其他目标文件格式忽略这个命令  
  符号 `__CTOR_LIST__` 标记全局构造函数的开始，`__CTOR_END__` 标记结束。类似的，`__DTOR_LIST__` 和 `__DTOR_END__` 标记了全局析构函数的开始和结束。列表中的第一个字是条目数，然后是每个构造函数或析构函数的地址，然后是零。编译器必须定义实际运行的代码。对于这些目标文件，`GNU C++` 通常调用子例程 `__main()` 的构造函数，它的调用会自动插入到 `main()` 开始处。`GNU C++` 自动运行析构函数，要么调用 `atexit()`，要么直接调用 `exit()`  
  对于支持任意段名的目标文件格式如 `COFF` 或 `ELF` ，`GNU C++` 通常将全局构造和析构函数的地址放入 `.ctors` 和 `.dtors` 段。讲以下序列放入链接器脚本，将构建 `GNU C++` 运行时的看起来那样的代码：  
  ```lds
  __CTOR_LIST__ = .;
  LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
  *(.ctors)
  LONG(0)
  __CTOR_END__ = .;
  __DTOR_LIST__ = .;
  LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
  *(.dtors)
  LONG(0)
  __DTOR_END__ = .;
  ```  
  如果你使用 `GNU C++` 来支持初始化优先级，它提供了对全局构造函数运行顺序的一些控制，你必须在链接时对构造函数进行排序，以确保它们以正确的顺序执行。当用了 `CONSTRUCTORS` 命令，请改用 `'SORT_BY_NAME(CONSTRUCTORS)'`。使用 `.ctors` 和 `.dtors` 段时，使用 `'*(SORT_BY_NAME(.ctors))'` 和 `'*(SORT_BY_NAME(.dtors))'` 而不仅仅是 `'*(.ctors)'` 和 `'*(.dtors)'`  
  通常编译器和链接器会自动处理这些问题，你不需要关心。但是，如果你用着 `C++` 并且写着你自己的链接器脚本，那还是要考虑的

<br></br>

### 弃用的输出段

> [原文地址](https://sourceware.org/binutils/docs/ld/Output-Section-Discarding.html)

链接器通常不会创建没有内容的输出文件，这是为了方便可能存在或不存在于任何输入文件中的输入段的检测，举个例子：

```lds
.foo : { *(.foo) }
```

只有在至少一个输入文件中有 `.foo` 段并且输入段不全为空时，才会在输出文件里创建一个 `.foo` 段。其他在一个输出段上分配空间的链接脚本指令也会创建输出段。**So too will assignments to dot even if the assignment does not create space, except for ‘. = 0’, ‘. = . + 0’, ‘. = sym’, ‘. = . + sym’ and ‘. = ALIGN (. != 0, expr, 1)’ when ‘sym’ is an absolute symbol of value 0 defined in the script.** 这使你可用 `'. = .'` 强制输出一个空段

链接器会忽略弃用输出段上的地址赋值（详见 [输出段地址](https://sourceware.org/binutils/docs/ld/Output-Section-Address.html)），除了当链接器脚本在输出段上定义了符号。这种情况下链接器会遵循地址赋值，**possibly advancing dot even though the section is discarded.**

特殊的输出段名 `'/DISCARD/'` 可用来丢弃输入段。一些赋值给 `'/DISCARD/'` 命名的输出段的输入段不会被最终的输出文件包含

这可用来丢弃用 `SHF_GNU_RETAIN` 这个 `ELF` 标识标记的输入段，否则这些部分会从链接器垃圾回收中保存

注意，匹配 `'/DISCARD/'` 的段会被丢弃，即使它们在一个不丢弃其他成员的 `ELF` 段组中。这是谨慎的，丢弃优先于分组

<br></br>

### 输出段属性

> [原文地址](https://sourceware.org/binutils/docs/ld/Output-Section-Attributes.html#Output-Section-Attributes)

之前我们已经给出了一个输出段的完整描述，像下面这样：

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
  } [>region] [AT>lma_region] [:phdr :phdr …] [=fillexp]
```

我们已经描述了 *`section`*、*`address`* 和 *`output-section-command`*，这里将描述剩下的段属性

---

#### 输出段类型

每个输出段可能都有一种类型，类型是一个括号括起来的关键字。下面这里类型是已经定义了的：

- `NOLOAD`
  标记某个段为不可加载的（*not loadable*），以便程序运行时该段不可以被加载
- `READONLY`
  标记某个段为只读
- `DSECT`
- `COPY`
- `INFO`
- `OVERLAY`
  上面这些关键字均支持向后兼容，并且都已经在使用了。它们都是同样的效果：标记某个段为不可分配的（*not allocatable*），以便当程序运行时不为这个段分配内存

链接器通常根据映射到输出段的输入段，设置输出段的属性。你可以用段类型来覆盖，比如，下面这个例子种，`ROM` 段起始于地址 `'0'` 处，并且程序运行时不需要加载

```lds
SECTIONS {
  ROM 0 (NOLOAD) : { … }
  …
}
```

<br></br>

#### 输出段 LMA

> [原文地址](https://sourceware.org/binutils/docs/ld/Output-Section-LMA.html)

每个段都有一个虚拟地址（VMA）和一个加载地址（LMA），详见 [脚本基本概念](https://sourceware.org/binutils/docs/ld/Basic-Script-Concepts.html)。虚拟地址由先前描述的 [输出段地址](https://sourceware.org/binutils/docs/ld/Output-Section-Address.html) 来指定。加载地址由 `AT` 或 `AT>` 关键字指定，指定加载地址是可选的

`AT` 关键字带上一个表达式作为参数，这指定了段的一个精确的加载地址。`AT>` 关键字带上一个内存区名作为参数，详见 [MEMORY](https://sourceware.org/binutils/docs/ld/MEMORY.html)。段的加载地址被设置为该内存区内下一个空闲地址，该地址对齐到所指定的对齐值上

如果一个可分配段（`allocatable section`）设置了要么 `AT` 要么 `AT>`，则链接器会遵循以下顺序来确定加载地址：

- 如果段指定了一个 `VMA` 地址，则这个地址也会用作 `LMA`
- 如果一个段是不可分配的（`not allocatable`），则其 `LMA` 地址会用来设置 `VMA`
- 否则，如果可以找到与当前段兼容的内存区，并且这个区域包含至少一个段，则 `LMA` 会被设置。此时 `VMA` 和 `LMA` 之间的差异与定位区中最后一个段的 `VMA` 和 `LMA` 之间的差异相同
- 如果没有已声明的内存区，则默认的内存区会在前一步使用，这个内存区覆盖了全部的地址空间
- 如没有找到合适的内存区，或者前面没有段可用，则 `LMA` 会设置为和 `VMA` 相等

这些特性被设计为更容易去搭建一个 `ROM` 镜像。比如，下面这个链接器脚本创建了三个输出段：一个起始地址为 `0x1000` 的 `.text`；一个是 `.mdata`，虽然它的 `VMA` 是 `0x2000`，但却是加载到 `.text` 后面；最后一个 `.bss` 用来在 `0x3000` 保存未初始化数据.符号 `_data` 用值 `0x2000` 定义，表示位置计数器保存的是 `VMA` 值而不是 `LMA` 值

```lds
SECTIONS
  {
  .text 0x1000 : { *(.text) _etext = . ; }
  .mdata 0x2000 :
    AT ( ADDR (.text) + SIZEOF (.text) )
    { _data = . ; *(.data); _edata = . ;  }
  .bss 0x3000 :
    { _bstart = . ;  *(.bss) *(COMMON) ; _bend = . ;}
}
```

与使用此链接器脚本生成的程序一起使用的运行时初始化代码将包括下面的内容，以将初始化数据从 `ROM` 镜像复制到其运行时地址。注意这份代码是怎样利用链接器脚本定义的符号的

```lds
extern char _etext, _data, _edata, _bstart, _bend;
char *src = &_etext;
char *dst = &_data;

/* ROM has data at end of text; copy it.  */
while (dst < &_edata)
  *dst++ = *src++;

/* Zero bss.  */
for (dst = &_bstart; dst< &_bend; dst++)
  *dst = 0;
```

<br></br>

#### 3.6.8.3 强制输出段对齐

> [原文地址](https://sourceware.org/binutils/docs/ld/Forced-Output-Alignment.html#Forced-Output-Alignment)

你可以利用 `ALIGN` 来增加一个输出段的对齐值。另一个替代方案是，利用 `ALIGN_WITH_INPUT` 属性来强制在整个输出段中保持 `VMA` 和 `LMA` 之间的差异不变

<br></br>

#### 3.6.8.4 强制输入段对齐

> [原文地址](https://sourceware.org/binutils/docs/ld/Forced-Input-Alignment.html#Forced-Input-Alignment)

你可以利用 `SUBALIGN` 来强制在输出段里面的输入段对齐，指定的值会覆盖输入段给定的任何对齐值，无论更大或是更小

<br></br>

#### 3.6.8.5 输出段约束

> [原文地址](https://sourceware.org/binutils/docs/ld/Output-Section-Constraint.html#Output-Section-Constraint)

你可以指定一个输出段，如果它的所有输入段分别只能通过 `ONLY_IF_RO` 和 `ONLY_IF_RW` 来设置只读和可读可写属性，则这种情况下才能创建该输出段

<br></br>

#### 3.6.8.6 输出段区域

> [原文地址](https://sourceware.org/binutils/docs/ld/Output-Section-Region.html#Output-Section-Region)

你可以将一个段赋值为前一个被 `>region` 定义的内存区，详见 [MEMORY](https://sourceware.org/binutils/docs/ld/MEMORY.html)

这里是一个简单的例子：

```lds
MEMORY { rom : ORIGIN = 0x1000, LENGTH = 0x1000 }
SECTIONS { ROM : { *(.text) } >rom }
```

<br></br>

#### 3.6.8.7 输出段 `Phdr`

> [原文地址](https://sourceware.org/binutils/docs/ld/Output-Section-Phdr.html#Output-Section-Phdr)

你可以将一个段赋值为前一个被 `':phdr;` 定义的程序段，详见 [PHDRS](https://sourceware.org/binutils/docs/ld/PHDRS.html)。如果一个段被赋值为一个或更多的段，则所有后续分配的段，也将被赋值给这些段，除非它们明确使用 `:phdr`。你可以用 `:NONE` 来告诉链接器不要在任何最终段里（`segment`）设置段（`section`）

这里是一个简单例子

```lds
PHDRS { text PT_LOAD ; }
SECTIONS { .text : { *(.text) } :text }
```

<br></br>

#### 3.6.8.8 输出段填充

> [原文地址](https://sourceware.org/binutils/docs/ld/Output-Section-Fill.html#Output-Section-Fill)

你可以用 `'=fillexp'` 为一个完整的段设置填充模式，`fillexp` 是一个表达式（详见 [表达式](https://sourceware.org/binutils/docs/ld/Expressions.html)）。否则在输出段里面任何未指定的内存区（比如由于输入段对齐而遗留的间隙）都会按需重复要填充的值。如果填充表达式是一个简单的十六进制数字（即一个以 `'0x'` 打头的数字字符串且不以 `'k'`、`'M'` 结尾），则一个任意长的十六进制数字序列会用来填充指定的填充模式，并且前导零也成为模式的一部分。其余所有情况，包括额外的括号或者一元运算符 `+`，填充模式是表达式值的四个最低有效字节。所有情况下，数字都是大端字节序

你也可以用 `FILL` 改变在输出段里的填充值，详见 [输出段数据](https://sourceware.org/binutils/docs/ld/Output-Section-Data.html)

这里是一个简单例子：

```lds
SECTIONS { .text : { *(.text) } =0x90909090 }
```

<br></br>

### 3.6.9 覆盖描述

> [原文地址](https://sourceware.org/binutils/docs/ld/Overlay-Description.html#Overlay-Description)

覆盖描述提供了一种更简单的方法来描述这种段：在相同的内存地址上加载单独的内存影响。在运行阶段，某种覆盖管理器将根据需要将覆盖段拷贝到运行时的内存地址，或从运行时的内存地址拷贝回来，这些操作可能只是简单地操作寻址位。当某个内存区比另一个更快时这种覆盖方法往往很有用

覆盖用 `OVERLAY` 描述，在 `SECTIONS` 命令中使用，和输出段描述一样。`OVERLAY` 整个语法如下：

```lds
OVERLAY [start] : [NOCROSSREFS] [AT ( ldaddr )]
  {
    secname1
      {
        output-section-command
        output-section-command
        …
      } [:phdr…] [=fill]
    secname2
      {
        output-section-command
        output-section-command
        …
      } [:phdr…] [=fill]
    …
  } [>region] [:phdr…] [=fill] [,]
```

除了 `OVERLAY` 以外其他所有都是可选的，同时每个段都必须要有一个名字（如上面 *`secname1`* 和 *`secname2`*）。在 `OVERLAY` 结构里面的段定义和普通 `SECTIONS` 结构（详见 [SECTIONS](https://sourceware.org/binutils/docs/ld/SECTIONS.html)）里面的段定义相同，只是 `OVERLAY` 里面不会为段定义地址和内存区

如果用了 *`fill`*，并且下一个 *`sections-command`* 看起来好像和前面的是连续的表达式时，则结尾处需要一个逗号

每个段都定义为同一个起始地址。段的加载地址在内存中是连续的，从用于整个 `OVERLAY` 的加载地址开始（与正常的段定义一样，加载地址是可选的，默认为起始地址；起始地址也是可选的，默认为位置计数器的当前值）

如果用了 `NOCROSSREFS`，并且在段里面有任何引用，则链接器会报告错误。因为段都运行在同一个地址，所以一个段直接引用另一个段通常没有意义（详见 [NOCROSSREFS](https://sourceware.org/binutils/docs/ld/Miscellaneous-Commands.html)）

对于每个在 `OVERLAY` 里面的段，链接器会自动提供两个符号。符号 `__load_start_secname` 定义为段的起始加载地址，`__load_stop_secname` 则定义为段的最终加载地址。*`secname`* 里面的任何不符合 `C` 标识符规范的字符都会被移除。`C`（或者汇编）代码可能会在需要的时候使用这些符号来移动覆盖段

在覆盖的最后，位置计数器的值会被设置为覆盖的起始地址加上最大的段的大小

这里是一个实例，记住这要放在 `SECTIONS` 描述里面

```lds
OVERLAY 0x1000 : AT (0x4000)
  {
    .text0 { o1/*.o(.text) }
    .text1 { o2/*.o(.text) }
  }
```

这会将 `.text0` 和 `.text1` 都定义在地址 `0x1000` 处，`.text0` 会加载到 `0x4000`，而 `.text1` 加载地址紧随其后。以下的符号会在引用时定义： `__load_start_text0`、`__load_stop_text0`、`__load_start_text1` 和 `__load_stop_text1`

将覆盖段 `.text1` 拷贝到覆盖区的 `C` 语言代码看起来可能是下面这个样子：

```lds
extern char __load_start_text1, __load_stop_text1;
memcpy ((char *) 0x1000, &__load_start_text1,
        &__load_stop_text1 - &__load_start_text1);
```

注意 `OVERLAY` 命令只是锦上添花，因为它所做的一切都可以使用更基本的命令来完成。上面这个例子也可以完全写成这样：

```lds
.text0 0x1000 : AT (0x4000) { o1/*.o(.text) }
PROVIDE (__load_start_text0 = LOADADDR (.text0));
PROVIDE (__load_stop_text0 = LOADADDR (.text0) + SIZEOF (.text0));
.text1 0x1000 : AT (0x4000 + SIZEOF (.text0)) { o2/*.o(.text) }
PROVIDE (__load_start_text1 = LOADADDR (.text1));
PROVIDE (__load_stop_text1 = LOADADDR (.text1) + SIZEOF (.text1));
. = 0x1000 + MAX (SIZEOF (.text0), SIZEOF (.text1));
```

<br></br>

## 3.7 MEMORY 命令

> [原文地址](https://sourceware.org/binutils/docs/ld/MEMORY.html)

链接器默认配置允许分配所有可用内存，你可以通过 `MEMORY` 覆盖该默认配置

`MEMORY` 命令描述了目标文件里内存的位置和块大小，你可以用 `MEMORY` 来指定链接器可以使用哪些内存区，避免使用哪些内存区，然后分配段到特定的内存区。链接器会设置基于内存区的段地址，报警空间即将用完的段。链接器不会优化段以适应可用内存区

一个链接器脚本可能会包含许多 `MEMORY` 命令，但是，所有定义的内存块都被视为在单个 `MEMORY` 命令中指定。`MEMORY` 语法如下：

```lds
MEMORY
{
  name [(attr)] : ORIGIN = origin, LENGTH = len
  …
}
```

`name` 是链接器脚本用来引用内存区的名字，离开脚本则该名字无意义。内存区名字储存在一个单独的名字空间，并且不会与其他符号名字、文件名字或段名冲突。每个内存区在 `MEMORY` 命令描述范围内必须有一个不同的名字，但是你也可以之后用 [`REGION_ALIAS`](https://sourceware.org/binutils/docs/ld/REGION_005fALIAS.html) 为一个已存在的内存区起别名

`attr` 字符串是一个可选的属性列表，用来指定是否为一个输入段使用特定的内存区，这些输入段在链接器脚本里没有显式的映射。像 [SECTIONS](https://sourceware.org/binutils/docs/ld/SECTIONS.html) 里描述的那样，如果你不为某些输入段指定输出段，则链接器会用于输入段相同的名字来创建一个输出段。如果你定义了内存区属性，则链接器会使用这些属性，为它创建的输出段选择内存区

`attr` 字符串只能由以下字符组成：

- `R`
  只读段
- `W`
  可读可写段
- `X`
  可执行段
- `A`
  可分配的（`Allocatable`）段
- `I`
  已初始化的（`Initialized`）段
- `t`
  和 `I` 相同
- `!`
  反转其后跟随的任何属性的含义

如果一个无映射的段匹配除了 `!` 以外的其他任何属性，则该段会被放入内存区。`!` 会反转属性，所以只有当无映射段与后面列出的任何属性都不匹配时，这才会将无映射段放入内存区中。因此，`'RW!X'` 属性字符串将匹配具有至少一个 `'R'` 和 `'W'` 属性的任何无映射段，但前提是这个段不具有 `X` 属性

`origin` 是一个针对内存区起始地址的数值表达式，该表达式在不调用任何符号的情况下，用来计算一个常数值。`ORIGIN` 关键字可以缩写为 `org` 或 `o`（但不能缩写为 `ORG`）

`len` 是一个以字节为单位的关于内存区大小的表达式。与 `origin` 表达式一样，该表达式只能是数值，并且是用来计算常数值的。`LENGTH` 关键字可以缩写成 `len` 或 `l`

在下面这个例子里，我们指定了两个可用与分配的内存区：一个在地址 `'0'` 处分配 `256 KB`，另一个在 `'0x0x40000000'` 处分配 `4 MB`。链接器会将无显式映射到内存区的每个段放入 `'ROM'`，属性为只读或可执行；而将其他无显式映射到内存区的所有段放入 `'RAM'`

```lds
MEMORY
{
  rom (rx)  : ORIGIN = 0, LENGTH = 256K
  ram (!rx) : org = 0x40000000, l = 4M
}
```

一旦你定义了一个内存区，你可以用 `'>region'` 输出段属性，指示链接器将特定的输出段放入这些内存区。比如，如果你有一个叫做 `'mem'` 的内存区，你可以在输出段定义里面用 `'>mem'`，详见 [输出段内存区](https://sourceware.org/binutils/docs/ld/Output-Section-Region.html)。如果没有为输出段指定地址，则链接器会在内存区内将当前地址设置为下一个可用地址。如果指向某个内存区的合并的输出段太大，链接器会报错

可以在一个表达式里，通过 `ORIGIN(memory)` 和 `LENGTH(memory)` 函数访问一个内存区起始地址和长度：

```lds
_fstack = ORIGIN(ram) + LENGTH(ram) - 4;
```

<br></br>

## 3.8 PHDRS 命令

`ELF` 目标文件格式使用 *`程序头（program headers）`*，也称为 `segments`，程序头描述了程序应该怎样被加载到内存。你可以通过 `objdump -p` 打印程序头

当你在原生 `ELF` 系统里运行一个 `ELF` 程序时，系统加载器会读取程序头以获悉怎样加载程序，当然前提是已经正确设置了程序头。但我们这份文档不会描述更多关于系统加载器解析程序头的细节，如果你想知道更多，可以参考 `ELF ABI`

链接器默认会创建合适的程序头。但是某些情况下，你可能需要指定更精确的程序头，此时就需要 `PHDRS` 命令了。当链接器在脚本里看见 `PHDRS` 命令，则不会创建你指定那个除外的其他程序头

当生成一个 `ELF` 输出文件时，链接器只关心 `PHDRS` 命令；不生成 `ELF` 时，链接器会简单忽略 `PHDRS`

以下是语法，`PHDRS`、`FILEHDR`、`AT` 和 `FLAGS` 都是关键字

```lds
PHDRS
{
  name type [ FILEHDR ] [ PHDRS ] [ AT ( address ) ]
        [ FLAGS ( flags ) ] ;
}
```

`name` 仅用于链接器脚本里 `SECTIONS` 命令中的引用，不会被放入输出文件。程序头名字储存在一个单独的名字空间，且不与任何符号名、文件名或段名冲突。每个程序头名字必须不同。程序头会被顺序处理，通常是按加载地址升序的顺序映射到段

某些程序头类型描述了系统加载器从文件中加载的内存段。在链接器脚本里，你可以通过在 `segment` 里放入可分配的输出段，来指定这些段的内容。你可以使用 `':phdr'` 输出段属性来将一个段放入一个特定的 `segment`，详见 [输出段 Phdr](https://sourceware.org/binutils/docs/ld/Output-Section-Phdr.html)


将某个段放入超过一个 `segment` 是很常见的，这仅仅意味着一个内存段包含另一个。只要有一个包含其他段的段，你就可以用一次 `':phdr'`

如果你在一个或多个 `segment` 中用 `':phdr'` 放入一个段，则链接器会在同一个 `segment` 中将随后那些没有指定 `':phdr'` 的段也放进来。这是十分方便的，因为通常一整个连续的段都是设置为单个 `segment`。你可以用 `:NONE` 来覆盖默认行为，并且告诉链接器不要将段放入任何 `segment` 中

你可以在程序头类型后面，用 `FILEHDR` 和 `PHDRS` 关键字进一步描述 `segment` 的内容。`FILEHDR` 关键字意味着 `segment` 应该包含 `ELF` 文件头（`ELF file header`）；`PHDRS` 关键字意味着 `segment` 应该包含它们自身的 `ELF` 程序头（`ELF program header`）。如果先前描述的所有可加载的 `segment`（`loadable segments`）都应用到同一个可加载 `segment`（`PT_LOAD`）上，则必须使用这些关键词里其中一个

`type` 可以是下面所示其中之一，数值指出了关键字的值：

- `PT_NULL (0)`
  指出一个不使用的程序头
- `PT_LOAD (1)`
  指出程序头描述了一个将从文件中加载的 `segment` 
- `PT_DYNAMIC (2)`
  指出一个 `segment` 能在哪里找到动态链接信息（`dynamic linking information`）
- `PT_INTERP (3)`
  指出一个 `segment` 能在哪里找到程序解析器的名字（`program interpreter`）
- `PT_NOTE (4)`
  指出一个保存注意信息（`note information`）的 `segment`
- `PT_SHLIB (5)`
  一个反转的程序头类型，由 `ELF ABI` 定义但不由它指定
- `PT_PHDR (6)`
  指出一个 `segment` 能在哪里找到程序头（`program header`）
- `PT_TLS (7)`
  指出一个包含线程本地储存（`thread local storage`）的 `segment`
- `expression`
  一个给出程序头数值类型的表达式，在非上面定义类型情况下使用

你可以通过使用一个 `AT` 表达式，来指定一个 `segment` 应该被加载到内存的哪一个特定地址上，这和在一个输出段属性（详见 [输出段 LMA](https://sourceware.org/binutils/docs/ld/Output-Section-LMA.html)）上使用 `AT` 命令是相同的。一个程序头的 `AT` 命令会覆盖输出段属性

链接器通常会基于组成 `segment` 的所有段来设置段属性。你可以用 `FLAGS` 关键字来显式指定段属性，但 *`flags`* 的值必须是一个整型值（`int`），用来设置程序头的 `p_flags` 字段

这里是一个 `PHDRS` 的例子，展示了一个在原生 `ELF` 系统上使用的，典型的程序头集合

```lds
PHDRS
{
  headers PT_PHDR PHDRS ;
  interp PT_INTERP ;
  text PT_LOAD FILEHDR PHDRS ;
  data PT_LOAD ;
  dynamic PT_DYNAMIC ;
}

SECTIONS
{
  . = SIZEOF_HEADERS;
  .interp : { *(.interp) } :text :interp
  .text : { *(.text) } :text
  .rodata : { *(.rodata) } /* defaults to :text */
  …
  . = . + 0x1000; /* move to a new page in memory */
  .data : { *(.data) } :data
  .dynamic : { *(.dynamic) } :data :dynamic
  …
}
```

<br></br>

## 3.9 VERSION 命令

> [原文地址](https://sourceware.org/binutils/docs/ld/VERSION.html)

当使用 `ELF` 文件时，链接器支持符号的版本。当使用动态库的时候符号版本才有意义。当动态链接器运行一个程序时，可能程序会链接到动态库的一个早期的版本，这是动态链接器可以使用符号版本来选择特定的函数版本

你可以直接在主要的链接器脚本里包含一个版本的脚本描述，或者隐式为链接器脚本提供版本的脚本描述，也可以用 `'--version-script'` 链接器选项

`VERSION` 语法十分简单

```lds
VERSION { version-script-commands }
```

版本脚本命令的格式和 `Solaris 2.5` 上使用 `Sun` 的链接器是一样的，它定义了一个版本结点树，你需要在版本脚本里指定结点名和依赖。你可以指定哪个符号绑定到哪个版本结点，并且你可以减少一个本地的特定符号集合，以便它们在共享库范围外不作为全局变量来使用

演示版本脚本的最简单方法是使用一些示例

```lds
VERS_1.1 {
	 global:
		 foo1;
	 local:
		 old*;
		 original*;
		 new*;
};

VERS_1.2 {
		 foo2;
} VERS_1.1;

VERS_2.0 {
		 bar1; bar2;
	 extern "C++" {
		 ns::*;
		 "f(int, double)";
	 };
} VERS_1.2;
```

这个实例版本脚本定义了三个版本结点。第一个版本结点是 `'VERS_1.1'`，这个版本没有其他依赖。这个脚本将符号 `'foo1'` 绑定了 `'VERS_1.1'`。它将许多符号作用范围缩减为本地，以便在共享库之外不可见；这是使用通配符模式完成的，因此任何名称以 `'old'`、`'original'` 或 `'new'` 打头的符号都会匹配。可用的通配符模式和那些在 `shell` 上匹配文件名使用的模式是相同的。然而，如果你在双引号里指定了符号名，则这个名字会被视为字面值（`literal`）而不是全局模式

第二个版本脚本定义了 `'VERS_1.2'` 结点，依赖于 `'VERS_1.1'`。脚本将符号 `'foo2'` 绑定了 `'VERS_1.2'` 结点

最后的版本脚本定义了 `'VERS_2.0'` 结点，依赖于 `'VERS_1.2'`。脚本将符号 `'bar1'` 和 `'bar2'` 绑定到版本节点 `'VERS_2.0'`

当链接器找到一个定义在库里的符号，没有指定应绑定哪一个版本结点时，则链接器会绑定该符号到一个未指定的基础库版本。你可以在版本脚本任意地方，用 `'global: *;'` 将所有其他未指定的符号绑定到给定的版本结点上。注意一般除了最后一个版本结点，其他结点的全局规范中都不会使用通配符。其他地方的全局通配符可能会意外地将符号添加到为旧版本导出的集合中。这是不对的，因为旧版本应该有另一组固定的符号

版本结点名除了可读性外，没有其他特别的含义。`'2.0'` 也可以出现在 `'1.1'` 和 `'1.2'` 之间，只是真这么写可能会在写版本脚本时造成迷惑

结点名可以删掉，前提是它是版本脚本中唯一的版本结点。这样一个版本脚本不会为符号分配任何版本，只会选择哪些符号是全局可见、哪些不可见

```lds
{ global: foo; bar; local: *; };
```

当你为一个应用链接一个拥有版本符号的动态库时，则应用本身知道每个符号要求的是哪个版本，并且也知道它链接的每个动态库需要的是哪个版本结点。因此在运行阶段，动态加载器会快速检查，以确保你已经链接的动态库实际上确实支持所有应用解析动态符号所需要的版本结点。通过这种方式，动态链接器可以确定它需要的所有外部符号都是可解析的，而无需搜索每个符号的引用

符号更新版本实际上是一种更复杂的方式，`SunOS` 用这种方式来对次要版本进行检查。这里要解决的基础问题通常是，由于外部函数的引用是按需绑定的，所以应用启动时并非是全部都绑定好。如果一个共享库过时了，那么一个所需的接口就会丢失，当应用尝试使用这个接口时，就会执行失败。随着符号更新版本，当用户启动该他们的程序时，如果应用使用的动态库太旧，则用户会收到一个警告

这里有一些 `GNU` 扩展，用以 `Sun` 版本更新。第一种方法是在定义符号的源文件中绑定符号到版本结点，而不是在更新脚本中。这么做主要是为了减轻动态库维护者的负担。你可以像下面这样

```lds
__asm__(".symver original_foo,foo@VERS_1.1");
```

写在 `C` 源文件中。这重命名了函数 `'original_foo'`，名字和绑定到版本结点 `'VERS_1.1'` 的 `'foo'` 别名一样。`'local:'` 指令可用于防止符号 `'original_foo'` 被导出。`'.symver'` 指令优先于版本脚本

第二个 `GNU` 扩展是允许同一个函数的多个版本出现在给定的共享库中。通过这种方式，你可以在不增加共享库的主要版本号的情况下对接口进行不兼容的更改，同时仍然允许链接到旧接口的应用中继续执行

你必须在源文件中用多个 `'.symver'` 指令，这里是一个实例：

```lds
__asm__(".symver original_foo,foo@");
__asm__(".symver old_foo,foo@VERS_1.1");
__asm__(".symver old_foo1,foo@VERS_1.2");
__asm__(".symver new_foo,foo@@VERS_2.0");
```

这个例子里面，`'foo@'` 表示符号 `'foo'` 绑定到符号的一个未指定的基础版本。包含这个例子的源文件会定义 4 个 `C` 函数：`'original_foo'`、`'old_foo'`、`'old_foo1'` 和 `'new_foo'`

当你的一个给定符号上有多个定义时，需要有某种方法来指定默认版本，该符号的外部引用将绑定到该版本。你可以用 `'.symver'` 指令的 `'foo@@VERS_2.0'` 类型。你只能以这种方式将符号的一个版本声明为默认版本，否则，你会对同一个符号产生重定义

如果你希望绑定一个引用到共享库中的符号的特定版本，你可以用别名（即 `'old_foo'`），或者你可以用 `'.symver'` 指令专门绑定到相关函数的外部版本

你也可以在版本脚本里指定语言：

```lds
VERSION extern "lang" { version-script-commands }
```

支持的 `'lang'` 是 `'C'`、`'C++'` 和 `'Java'`。链接器会在链接时迭代符号列表并根据 `'lang'` 对它们进行解构，然后将它们与 version-script-commands 中指定的模式匹配，默认的 `'LANG'` 是 `'C'`

解构名字可能包含空格和其他特殊字符。像上面描述那样，你可以用一个全局模式来匹配解构后的名称，也可以用双引号字符串来精确地匹配字符串。后一种情况里，要注意版本脚本和解构器的输出之间微小的差异（比如不同的空格）也会导致不匹配。由于解构器生成的确切字符串将来可能会更改，即使损坏的名称没有更改，你也应该在升级时检查所有版本指令的行为是否符合你的预期

<br></br>

## 3.10 链接器脚本里的表达式

> [原文地址](https://sourceware.org/binutils/docs/ld/Expressions.html)

在连接器脚本里面表达式的语法和 `C` 表达式是一样的，除了在某些地方需要用空白字符来解决语法歧义

