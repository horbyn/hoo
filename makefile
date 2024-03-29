# SSRC: .s 集合，利用 find 命令找到当前路径及以下所有子路径下的 .s
# CSRC: .c 集合
# OBJS: 将 SSRC 和 CSRC 两个集合换成 .o，然后只筛掉 bootsect.o
#       详见 https://seisman.github.io/how-to-write-makefile/functions.html
#       以及 https://stackoverflow.com/questions/33740270/makefile-patsubst-multiple-expressions
SSRC := $(shell find . -name "*.s")
CSRC := $(shell find . -name "*.c")
OBJS := $(filter-out ./boot/bootsect.o, $(SSRC:.s=.o))
OBJC := $(CSRC:.c=.o)

AS := as
LD := ld
CC := gcc

# -c: 编译
# -Wall: 显示所有警告
# -Werror: 把警告当成错误
# -m32: 生成可以运行在任何 i386 环境的 32 位代码
#       详见 https://gcc.gnu.org/onlinedocs/gcc/x86-Options.html#x86-Options
# -nostdinc: 只有在指令通过选项（-I, -iquote..）情况下，其余情况均不使用标准库函数
#       详见 https://gcc.gnu.org/onlinedocs/gcc/Directory-Options.html#Directory-Options
# -fno-builtin: 不允许编译器优化我们定义的函数
#       详见 https://gcc.gnu.org/onlinedocs/gcc/C-Dialect-Options.html#C-Dialect-Options
CFLAGS := -c -Wall -Werror -m32 -nostdinc -fno-builtin -fno-pie -fno-stack-protector
# -m: 指定输出文件的格式
# -Map: 输出内存映射文件
LDFLAGS := -m elf_i386 -Map kernel.map

# 第一条命令请保持依赖为 img，这样默认 make 就能执行
nop: clean image $(OBJS) $(OBJC) fd1_44M.img
	DISPLAY=:0 /usr/bin/bochs

debug: CFLAGS += -g -DDEBUG
debug: clean image $(OBJS) $(OBJC) fd1_44M.img
	DISPLAY=:0 /usr/bin/bochs

run:
	DISPLAY=:0 /usr/bin/bochs

# -f 文件存在就不生成
#     详见 man test
image:
	if [ ! -f "fd1_44M.img" ]; then \
		dd if=/dev/zero of=fd1_44M.img bs=1474560 count=1; \
	fi

# objdump -S: 混合源代码的形式反汇编代码段
#         -D: 反汇编所有段
fd1_44M.img: bootsect kernel.elf
	dd if=bootsect of=fd1_44M.img bs=512 count=1 conv=notrunc
	objcopy -S -O binary kernel.elf kernel
	dd if=kernel of=fd1_44M.img bs=512 count=896 seek=1 conv=notrunc
	objdump -SD -m i386 kernel.elf > kernel.elf.d

# --oformat: 输出格式为纯二进制
# -e: bootsect 入口点为 _start
# -Ttext: 链接地址总是加上这个数值
# -m: 模拟一个编译环境，当需要在 64 位机器上编译 32 位代码时需要模拟 32 位编译环境
#     详见 https://stackoverflow.com/questions/19200333/architecture-of-i386-input-file-is-incompatible-with-i386x86-64
bootsect: ./boot/bootsect.o
	$(LD) --oformat binary -e _start -Ttext 0x7c00 -m elf_i386 $< -o $@

./boot/bootsect.o: ./boot/bootsect.s
	$(AS) --32 $< -o $@

kernel.elf: $(OBJS) $(OBJC)
	$(LD) $(LDFLAGS) -T kernel.ld $^ -o $@

# OBJS 集合中的所有 .o 都需要 .s(.c) 作前提
# --32: 指定生成 32 位字长，即隐含目标平台（target）为 Intel i386 架构
#       https://sourceware.org/binutils/docs/as/i386_002dOptions.html#i386_002dOptions
$(OBJS): %.o: %.s
	$(AS) --32 $< -o $@

$(OBJC): %.o: %.c
	$(CC) $(CFLAGS) $< -o $@

# -rm: 有些文件可能不存在，但不用管（但还是会报错的）
clean:
	-rm -r $(OBJS) $(OBJC) ./boot/bootsect.o bootsect \
	./kernel ./kernel.elf ./kernel.elf.d ./kernel.map fd1_44M.img
