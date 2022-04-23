AS=as
LD=ld
OBJS=bootsect.o

# 第一条命令请保持依赖为 img，这样默认 make 就能执行
# DISPLAY=:0，由于启动 bochs 需要 GUI，所以启动需要加上这个
nop: fd1_44M.img
	DISPLAY=:0 /usr/bin/bochs

fd1_44M.img: bootsect
	dd if=bootsect of=fd1_44M.img bs=512 count=1 conv=notrunc

clean:
	rm $(OBJS)

# --oformat: 输出格式为纯二进制
# -e: bootsect 入口点为 _start
# -Ttext: 链接地址总是加上这个数值
# -m: 模拟一个编译环境，当需要在 64 位机器上编译 32 位代码时需要模拟 32 位编译环境
#     详见 https://stackoverflow.com/questions/19200333/architecture-of-i386-input-file-is-incompatible-with-i386x86-64
bootsect: bootsect.o
	$(LD) --oformat binary -e _start -Ttext 0x7c00 -m elf_i386 $< -o bootsect

# --32: https://sourceware.org/binutils/docs/as/i386_002dOptions.html#i386_002dOptions
$(OBJS): %.o: %.s
	$(AS) --32 $< -o $@