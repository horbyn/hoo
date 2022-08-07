# bootloader

## x86 故事开始的地方

当按下开机键那一刻，对于 `x86` 硬件来说发生了很多事，但对于内核来说，发生的第一件事便是跳转到 `0:0x7c00` 处执行指令

为什么这个地方会有指令？要知道内存属于 RAM，刚开机是没有数据的。其实是 `BIOS` 替内核作了一些处理

`BIOS` 是一个固件，烧录到 ROM 的，所有总能找到 `BIOS`。它从存储系统读取了一些数据，加载到内存 `0:0x7c00`，因此对于内核来说就有了**第一条指令**

再具体点，`BIOS` 加载的这个 "数据"，被称为 `MBR（主引导记录，Master Boot Record，或者主引导扇区）`，它总是位于存储系统的某个固定的地方（对于磁盘来说是第一个逻辑扇区）。无论刮风还是下雨，`MBR` 都矢志不渝地在这个地方等待着 `BIOS` 的呼唤

![](https://pic.imgdb.cn/item/62ee33588c61dc3b8ec5c251.jpg)

`MBR` 是**整个内核故事的开始**，换句话说，如果你想实现一个内核，这便是第一个程序（当然，这个阶段你也可以不列入内核功能的范畴，你可以将这个阶段交给其他机制/技术来完成，比如 `GRUB`、`UEFI` 等等）。如果你要实现 `MBR`，首先要明确以下这几点限制：

- 大小限制为 512 字节
- 最后两个字节先是 `0x55`，然后是 `0xaa`
- 需要使用 CPU 能直接识别的机器指令来编写

由于大小限制得太严重，所以内核不可能放在 `MBR` 里面实现，传统的做法是：

- 先加载 `MBR`，让 `MBR` 去加载位于存储系统的另一个程序
- 然后这个程序做一些初始化工作（打开和进入保护模式、开启页表机制等等），之后才从存储系统加载真真正正内核

在传统的做法里，`MBR` 被称为引导器（booter），而这里的 "另一个程序" 被称为加载器（loader），然后把它们统称为 `bootloader`，这便是引导和加载

之所以称为 "传统"，一方面是因为大多数资料都是这样的过程（[《于渊，自动动手写操作系统》](https://book.douban.com/subject/1422377/)，[《郑钢，操作系统真相还原》](https://book.douban.com/subject/26745156/) 甚至 [《赵炯，Linux 内核 0.11 详细注释》](https://mirror.math.princeton.edu/pub/oldlinux/download/clk011.pdf) 等等）；另一方面是我这里不会遵循这个流程，我的原因是：

- 引导和加载本质上都是加载其他的程序，让其他程序去完成某些功能
- 引导和加载需要使用 CPU 能直接识别的机器指令来编写，比如汇编（当然也可以用高级语言来编写，之后使用诸如 `objcopy` 等命令为生成的可执行文件提取纯粹的二进制指令）

首先，`booter` 可能只能用汇编来写，因为要控制大小为 512 字节且最后两个字节为 `0x55` 和 `0xaa`

然后，`loader` 要做的初始化工作其实是很多的，我并不想用汇编写这么多东西

所以，我的思路是，我会把 `loader` 删掉。但是初始化工作还是要做的，但不是汇编来做，而是用高级语言来做（`C 语言`）。所以我会直接从 `boot` 一下子跳入内核，让内核自己做它自己的初始化工作

<br></br>

## 从汇编到 C 语言

如果是将汇编源文件和 C 源文件链接在一起，那么问题会变得很简单，只需要把 C 的函数名声明为链接阶段可见就行了，比如在汇编源文件里用 `.globl` 声明一个函数名

但是现在的情况是，`booter` 是单独的一个程序，而我们自己实现的内核则是另外一个程序。换句话说，这两个程序不能链接到一起，那么你在汇编里跳入一个 C 的函数名，这就会报错 *找不到符号*

让我们先来看看 `Linux 0.11` 是怎么做的（[详见《赵炯 linux 0.11，第 2 章》](https://mirror.math.princeton.edu/pub/oldlinux/download/clk011.pdf)，源码见 [oldlinux 的镜像](https://mirror.cs.msu.ru/oldlinux.org/Linux.old/Linux-0.11/sources/system/index.html)）：

>
> ![](https://pic.imgdb.cn/item/62ef782916f2c2beb15fd43e.png)
>
> 这是 `Linux 0.11` 从系统加电起所执行的程序的顺序，主要关注控制流改变的地方（即箭头处）：
>
> - 第一个箭头：由 `BIOS` 控制
> - 第二个箭头（从 `bootsect.S` 到 `setup.s`）：`bootsect.S` 执行 [段间跳转](https://github.com/karottc/linux-0.11/blob/f8d044e078f5e5ee20a3ad2f72c243f041526983/boot/bootsect.s#L139)，其中，符号 `SETUPSEG`（ [值为 0x9020](https://github.com/karottc/linux-0.11/blob/f8d044e078f5e5ee20a3ad2f72c243f041526983/boot/bootsect.s#L37) ）表示 `setup` 模块的起始地址
>     + `jmpi	0,SETUPSEG`，该指令表示跳转至 `0x9020: 0` 处执行
> - 第三个箭头（从 `setup.s` 到 `system 模块`）：`setup.s` 同样是执行 [段间跳转](https://github.com/karottc/linux-0.11/blob/f8d044e078f5e5ee20a3ad2f72c243f041526983/boot/setup.s#L191)，不过需要注意此时已经进入了保护模式，源操作数 `8` 表示的是段选择子
>     + `jmpi	0,8`，该指令表示跳入绝对地址 `0: 0` 处执行
> - 最后一个箭头：由于 `head.s` 和 `main.c` 是链接为一起的，所以 `head.s` 是能够 "看见" `main.c` 的函数名的，所以这里的跳转就很简单了
>
> 当然，上面只是程序源码的内容，编译过程输入了什么命令也很重要，因为也能控制执行流的改变，所以接下来看看 `Makefile` 是怎么写的：
>
> - 生成 `bootsect` 模块：主要关注下面这条命令给出的 [链接过程](https://github.com/karottc/linux-0.11/blob/f8d044e078f5e5ee20a3ad2f72c243f041526983/Makefile#L93)，可以发现 `bootsect` 通过 `-O` 选项（ [展开 LD86 变量](https://github.com/karottc/linux-0.11/blob/f8d044e078f5e5ee20a3ad2f72c243f041526983/Makefile#L8) ）直接生成了 `8086` 二进制文件
>     + `$(LD86) -s -o boot/bootsect boot/bootsect.o`
> - 生成 `setup` 模块：和 `bootsect` 是[同样的命令](https://github.com/karottc/linux-0.11/blob/f8d044e078f5e5ee20a3ad2f72c243f041526983/Makefile#L91) 生成的是 `8086` 二进制文件
>     + `$(LD86) -s -o boot/setup boot/setup.o`
> - 生成 `system` 模块：[主要关注链接命令](https://github.com/karottc/linux-0.11/blob/f8d044e078f5e5ee20a3ad2f72c243f041526983/Makefile#L58)，这里有个链接顺序的问题，可以看到内核模块是把 `boot/head.o` 和 `init/main.o` 放在前面，这就保证了从 `setup` 模块跳入内核时先执行的 `head` 模块的指令
>     + `$(LD) $(LDFLAGS) boot/head.o init/main.o \`
>     + 这里 [展开 LDFLAGS 变量](https://github.com/karottc/linux-0.11/blob/f8d044e078f5e5ee20a3ad2f72c243f041526983/Makefile#L12) 可以看到内核模块是生成的 `ELF` 二进制文件

至此，便可以总结一下 `Linux 0.11` 的 `bootloader` 过程（对于 `0.11` 来说，`setup` 模块就是 `loader`）

- 分别生成 `bootsect`、`setup` 和 `内核` 单独的三个模块
- 各个模块之间的跳转利用段间跳转实现，涉及的地址为绝对地址
- 这里的关键是链接的过程

值得一提的是，[xv6-x86-fall-2018](https://pdos.csail.mit.edu/6.828/2018/xv6/xv6-rev10.pdf) 和 [ucore-x86-fall-2018](https://github.com/xr1s/ucore_os_lab/tree/master/labcodes/lab2) 这两个比较出名的教学内核也是用了类似的 `booter` -> `loader` -> `kernel` 的手段，这说明天下技术均 "一家" ？（XD）

这可以给我们 **从汇编跳入 C** 提供这样的思路（假设我们希望内核的入口点为 `0x100000`）：

- 链接后的输出文件的起始地址指定为 `0x100000`（可以用 `ld -Ttext` 选项也可以用链接脚本完成）
- 输入文件的链接顺序一定要指定 C 文件放开头，这样这个 C 文件编译后的 `.text` 便相当于 "绑定" 到地址 `0x100000` 了（换句话说，第一个 C 函数的地址就是 `0x100000`）
- 最后只需要在汇编源文件（即 `MBR`）上利用段间跳转，跳转到 `0x100000` 就可以了
