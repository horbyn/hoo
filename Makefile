BOOT_IMG := fd1_44M.img
DISK     := hd.img
SIZE_SEC := 512

# SSRC：    `.S` 文件集合，使用 `find` 命令查找当前目录及其所有子目录下的 `.S` 文件
# CSRC：    `.c` 文件集合
# OBJ{S,C}：将 `SSRC`（`CSRC`）转换为 `.o` 文件集合，然后过滤掉 `bootsect.o`，详见
#           https://seisman.github.io/how-to-write-makefile/functions.html
#           https://stackoverflow.com/questions/33740270/makefile-patsubst-multiple-expressions
SSRC := $(shell find . -name "*.S")
CSRC := $(shell find . -name "*.c")
OBJS := $(filter-out ./boot/bootsect.o, $(SSRC:.S=.o))
OBJC := $(filter-out ./user/null.o, $(CSRC:.c=.o))

LD := ld
CC := gcc

INC := -I./

# -c:      编译
# -Wall：  输出所有警告
# -Werror：将警告视为错误
# -m32：   生成可以在任何 i386 架构机器上执行的 32 位源代码，详见：
#          https://gcc.gnu.org/onlinedocs/gcc/x86-Options.html#x86-Options
# -nostdinc：不使用标准函数，除非指定某些选项（`-I`、`-iquote` 等），详见：
#           https://gcc.gnu.org/onlinedocs/gcc/Directory-Options.html#Directory-Options
# -fno-builtin：不允许编译器优化我们自己的函数，详见：
#               https://gcc.gnu.org/onlinedocs/gcc/C-Dialect-Options.html#C-Dialect-Options
# -fno-pie：禁止生成 `__x86.get_pc_thunk.ax` 子程序，详见：
#           https://stackoverflow.com/questions/50105581/how-do-i-get-rid-of-call-x86-get-pc-thunk-ax
# -fno-stack-protector：禁用可以避免生成一些未定义的符号（例如 `__stack_chk_fail`）
CFLAGS := -c -Wall -Werror -m32 $(INC) \
	-nostdinc -fno-builtin -fno-pie -fno-stack-protector -Wno-main
# -m：  指定输出格式
# -Map：输出内存映射文件
LDFLAGS := -m elf_i386 -Map kernel.map

# 保持 `.img` 依赖在第一个命令中，这样 `make` 将默认执行所有命令
nop: boot_image $(OBJS) $(OBJC) $(BOOT_IMG)

debug: CFLAGS += -g
debug: boot_image $(OBJS) $(OBJC) $(BOOT_IMG)

bochs:
	bochs -q

qemu:
	qemu-system-i386 -smp 1 -m 32M -nographic -boot a -fda $(BOOT_IMG) \
		-drive file=$(DISK),media=disk,index=0,format=raw

qemu-debug:
	qemu-system-i386 -smp 1 -m 32M -nographic -boot a -fda $(BOOT_IMG) \
		-drive file=$(DISK),media=disk,index=0,format=raw -S -gdb tcp::12345

# -f：如果文件存在则不生成
# 1.44M floppy: 80(C) * 2(H) * 18(S) * 512 =   1,474,560
boot_image:
	if [ ! -f "$(BOOT_IMG)" ]; then \
		dd if=/dev/zero of=$(BOOT_IMG) bs=1474560 count=1; \
	fi

# 硬盘: 1057478 * 512 = 541,428,736
format_disk:
	-rm -r $(DISK)
	dd if=/dev/zero of=$(DISK) bs=541428736 count=1

SEG                  := -j .text -j .rodata -j .data
SIZE_KERNEL          := 499
SIZE_BUILTIN         := 50
BASE_SEC_BUILTIN_SH  := $(SIZE_KERNEL) + 1
BASE_SEC_BUILTIN_CD  := $(BASE_SEC_BUILTIN_SH) + $(SIZE_BUILTIN)
BASE_SEC_BUILTIN_LS  := $(BASE_SEC_BUILTIN_CD) + $(SIZE_BUILTIN)
BASE_SEC_BUILTIN_PWD := $(BASE_SEC_BUILTIN_LS) + $(SIZE_BUILTIN)
BASE_SEC_BUILTIN_MKD := $(BASE_SEC_BUILTIN_PWD) + $(SIZE_BUILTIN)
BASE_SEC_BUILTIN_TOU := $(BASE_SEC_BUILTIN_MKD) + $(SIZE_BUILTIN)
BASE_SEC_BUILTIN_RM  := $(BASE_SEC_BUILTIN_TOU) + $(SIZE_BUILTIN)
BASE_BUILTIN_SH      := $(shell echo $$((  (($(BASE_SEC_BUILTIN_SH) - 2) * $(SIZE_SEC))  )))
END_BUILTIN_SH       := $(shell echo $$((  (($(BASE_SEC_BUILTIN_SH) + $(SIZE_BUILTIN) - 2) * $(SIZE_SEC) - 1)  )))
BASE_BUILTIN_CD      := $(shell echo $$((  (($(BASE_SEC_BUILTIN_CD) - 2) * $(SIZE_SEC))  )))
END_BUILTIN_CD       := $(shell echo $$((  (($(BASE_SEC_BUILTIN_CD) + $(SIZE_BUILTIN) - 2) * $(SIZE_SEC) - 1)  )))
BASE_BUILTIN_LS      := $(shell echo $$((  (($(BASE_SEC_BUILTIN_LS) - 2) * $(SIZE_SEC))  )))
END_BUILTIN_LS       := $(shell echo $$((  (($(BASE_SEC_BUILTIN_LS) + $(SIZE_BUILTIN) - 2) * $(SIZE_SEC) - 1)  )))
BASE_BUILTIN_PWD     := $(shell echo $$((  (($(BASE_SEC_BUILTIN_PWD) - 2) * $(SIZE_SEC))  )))
END_BUILTIN_PWD      := $(shell echo $$((  (($(BASE_SEC_BUILTIN_PWD) + $(SIZE_BUILTIN) - 2) * $(SIZE_SEC) - 1)  )))
BASE_BUILTIN_MKDIR   := $(shell echo $$((  (($(BASE_SEC_BUILTIN_MKD) - 2) * $(SIZE_SEC))  )))
END_BUILTIN_MKDIR    := $(shell echo $$((  (($(BASE_SEC_BUILTIN_MKD) + $(SIZE_BUILTIN) - 2) * $(SIZE_SEC) - 1)  )))
BASE_BUILTIN_TOUCH   := $(shell echo $$((  (($(BASE_SEC_BUILTIN_TOU) - 2) * $(SIZE_SEC))  )))
END_BUILTIN_TOUCH    := $(shell echo $$((  (($(BASE_SEC_BUILTIN_TOU) + $(SIZE_BUILTIN) - 2) * $(SIZE_SEC) - 1)  )))
BASE_BUILTIN_RM      := $(shell echo $$((  (($(BASE_SEC_BUILTIN_RM) - 2) * $(SIZE_SEC))  )))
END_BUILTIN_RM       := $(shell echo $$((  (($(BASE_SEC_BUILTIN_RM) + $(SIZE_BUILTIN) - 2) * $(SIZE_SEC) - 1)  )))

# objdump -S：反汇编源代码中的 .text 段，并且输出源代码和反汇编代码
#         -D：反汇编所有段
#         -j：指定生成的段
$(BOOT_IMG): bootsect kernel.elf sh.elf cd.elf ls.elf pwd.elf mkdir.elf touch.elf rm.elf
	dd if=bootsect of=$(BOOT_IMG) bs=$(SIZE_SEC) count=1 conv=notrunc
	objcopy -S -O binary kernel.elf kernel
	dd if=kernel of=$(BOOT_IMG) bs=$(SIZE_SEC) count=$(SIZE_KERNEL) seek=1 conv=notrunc
	objcopy -S -O binary user/sh.elf user/sh
	dd if=user/sh of=$(BOOT_IMG) bs=$(SIZE_SEC) count=$(SIZE_BUILTIN) \
		seek=$(shell echo $$((  $(BASE_SEC_BUILTIN_SH) - 1  ))) conv=notrunc
	objcopy -S -O binary user/cd.elf user/cd
	dd if=user/cd of=$(BOOT_IMG) bs=$(SIZE_SEC) count=$(SIZE_BUILTIN) \
		seek=$(shell echo $$((  $(BASE_SEC_BUILTIN_CD) - 1  ))) conv=notrunc
	objcopy -S -O binary user/ls.elf user/ls
	dd if=user/ls of=$(BOOT_IMG) bs=$(SIZE_SEC) count=$(SIZE_BUILTIN) \
		seek=$(shell echo $$((  $(BASE_SEC_BUILTIN_LS) - 1  ))) conv=notrunc
	objcopy -S -O binary user/pwd.elf user/pwd
	dd if=user/pwd of=$(BOOT_IMG) bs=$(SIZE_SEC) count=$(SIZE_BUILTIN) \
		seek=$(shell echo $$((  $(BASE_SEC_BUILTIN_PWD) - 1  ))) conv=notrunc
	objcopy -S -O binary user/mkdir.elf user/mkdir
	dd if=user/mkdir of=$(BOOT_IMG) bs=$(SIZE_SEC) count=$(SIZE_BUILTIN) \
		seek=$(shell echo $$((  $(BASE_SEC_BUILTIN_MKD) - 1  ))) conv=notrunc
	objcopy -S -O binary user/touch.elf user/touch
	dd if=user/touch of=$(BOOT_IMG) bs=$(SIZE_SEC) count=$(SIZE_BUILTIN) \
		seek=$(shell echo $$((  $(BASE_SEC_BUILTIN_TOU) - 1  ))) conv=notrunc
	objcopy -S -O binary user/rm.elf user/rm
	dd if=user/rm of=$(BOOT_IMG) bs=$(SIZE_SEC) count=$(SIZE_BUILTIN) \
		seek=$(shell echo $$((  $(BASE_SEC_BUILTIN_RM) - 1  ))) conv=notrunc
	objdump $(SEG) -SD -m i386 kernel.elf > kernel.elf.dis
	objdump $(SEG) -SD -m i386 user/sh.elf >> kernel.elf.dis
	objdump $(SEG) -SD -m i386 user/cd.elf >> kernel.elf.dis
	objdump $(SEG) -SD -m i386 user/ls.elf >> kernel.elf.dis
	objdump $(SEG) -SD -m i386 user/pwd.elf >> kernel.elf.dis
	objdump $(SEG) -SD -m i386 user/mkdir.elf >> kernel.elf.dis
	objdump $(SEG) -SD -m i386 user/touch.elf >> kernel.elf.dis
	objdump $(SEG) -SD -m i386 user/rm.elf >> kernel.elf.dis

# --oformat：输出纯净的二进制格式
# -e：       默认入口是_start，但这个选项可以指定其他入口
# -Ttext：   链接地址总是加上这个值
# -m：       当使用 64 位机器编译 32 位代码时，模拟 32 位编译环境，详见
#            https://stackoverflow.com/questions/19200333/architecture-of-i386-input-file-is-incompatible-with-i386x86-64
bootsect: ./boot/bootsect.o
	$(LD) --oformat binary -e _start -Ttext 0x7c00 -m elf_i386 $< -o $@

./boot/bootsect.o: ./boot/bootsect.S
	$(CC) $(CFLAGS) $< -o $@

kernel.elf: $(OBJS) $(OBJC)
	$(LD) $(LDFLAGS) -T kernel.ld $^ -o $@

sh.elf: user/builtin_shell.o user/user.o
	$(LD) -m elf_i386 -e main_shell -T user/builtin.ld $^ -o user/$@

cd.elf: user/builtin_cd.o user/user.o
	$(LD) -m elf_i386 -e main_cd -T user/builtin.ld $^ -o user/$@

ls.elf: user/builtin_ls.o user/user.o
	$(LD) -m elf_i386 -e main_ls -T user/builtin.ld $^ -o user/$@

pwd.elf: user/builtin_pwd.o user/user.o user/lib.o user/null.o
	$(LD) -m elf_i386 -e main_pwd -T user/builtin.ld $^ -o user/$@

mkdir.elf: user/builtin_mkdir.o user/user.o user/lib.o user/null.o
	$(LD) -m elf_i386 -e main_mkdir -T user/builtin.ld $^ -o user/$@

touch.elf: user/builtin_touch.o user/user.o user/lib.o user/null.o
	$(LD) -m elf_i386 -e main_touch -T user/builtin.ld $^ -o user/$@

rm.elf: user/builtin_rm.o user/user.o user/lib.o user/null.o
	$(LD) -m elf_i386 -e main_rm -T user/builtin.ld $^ -o user/$@

CFLAGS += -D__BASE_BUILTIN_SH=$(BASE_BUILTIN_SH) \
	-D__END_BUILTIN_SH=$(END_BUILTIN_SH) \
	-D__BASE_BUILTIN_CD=$(BASE_BUILTIN_CD) \
	-D__END_BUILTIN_CD=$(END_BUILTIN_CD) \
	-D__BASE_BUILTIN_LS=$(BASE_BUILTIN_LS) \
	-D__END_BUILTIN_LS=$(END_BUILTIN_LS) \
	-D__BASE_BUILTIN_PWD=$(BASE_BUILTIN_PWD) \
	-D__END_BUILTIN_PWD=$(END_BUILTIN_PWD) \
	-D__BASE_BUILTIN_MKDIR=$(BASE_BUILTIN_MKDIR) \
	-D__END_BUILTIN_MKDIR=$(END_BUILTIN_MKDIR) \
	-D__BASE_BUILTIN_TOUCH=$(BASE_BUILTIN_TOUCH) \
	-D__END_BUILTIN_TOUCH=$(END_BUILTIN_TOUCH) \
	-D__BASE_BUILTIN_RM=$(BASE_BUILTIN_RM) \
	-D__END_BUILTIN_RM=$(END_BUILTIN_RM)

$(OBJS): %.o: %.S
	$(CC) $(CFLAGS) $< -o $@

$(OBJC): %.o: %.c
	$(CC) $(CFLAGS) $< -o $@

# -rm：有些文件不能不存在，-rm 表示忽略这些文件
clean:
	-rm -r $(OBJS) $(OBJC) ./boot/bootsect.o user/null.o     \
	bootsect  ./kernel ./kernel.elf ./*.dis ./kernel.map     \
	$(BOOT_IMG)  user/*.elf user/sh user/cd user/ls user/pwd \
	user/mkdir user/touch user/rm
