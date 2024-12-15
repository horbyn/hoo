BOOT_IMG := fd1_44M.img
DISK     := hd.img
SIZE_SEC := 512

# SSRC: the `.S` set, which is used the `find` command to find out all the `.S`
#       in current dir and all the recursive sub-dir
# CSRC: the `.c` set
# OBJS: combine `SSRC` and `CSRC` to be the `.o` set, then filters `bootsect.o`
#       DETIALS IN https://seisman.github.io/how-to-write-makefile/functions.html
#       https://stackoverflow.com/questions/33740270/makefile-patsubst-multiple-expressions
SSRC := $(shell find . -name "*.S")
CSRC := $(shell find . -name "*.c")
OBJS := $(filter-out ./boot/bootsect.o, $(SSRC:.S=.o))
OBJC := $(filter-out ./user/null.o, $(CSRC:.c=.o))

LD := ld
CC := gcc

INC := -I./

# -c: compile
# -Wall: display all the warnings
# -Werror: regard the warnings to errors
# -m32: generate 32 bit source can be executed in any i386 arch machine
#       DETIALS IN https://gcc.gnu.org/onlinedocs/gcc/x86-Options.html#x86-Options
# -nostdinc: dont use standard functions unless specify some options(`-I`, `-iquote` etc.)
#       DETIALS IN https://gcc.gnu.org/onlinedocs/gcc/Directory-Options.html#Directory-Options
# -fno-builtin: dont allow the compiler to optimize our own function
#       DETIALS IN https://gcc.gnu.org/onlinedocs/gcc/C-Dialect-Options.html#C-Dialect-Options
# -fno-pie: forbid to generate `__x86.get_pc_thunk.ax` subroutine
#       DETIALS IN https://stackoverflow.com/questions/50105581/how-do-i-get-rid-of-call-x86-get-pc-thunk-ax
# -fno-stack-protector: enable will generate some undefined symbols (e.g. __stack_chk_fail)
CFLAGS := -c -Wall -Werror -m32 $(INC) \
	-nostdinc -fno-builtin -fno-pie -fno-stack-protector -Wno-main
# -m: specify the output format
# -Map: output memory map
LDFLAGS := -m elf_i386 -Map kernel.map

# keep the depencement `.img` in the first command pleaze, that `make` will
#    execute all the command by default
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

# -f: dont generate the file if exists
# 1.44M floppy: 80(C) * 2(H) * 18(S) * 512 =   1,474,560
boot_image:
	if [ ! -f "$(BOOT_IMG)" ]; then \
		dd if=/dev/zero of=$(BOOT_IMG) bs=1474560 count=1; \
	fi

# hard disk:	             1057478 * 512 = 541,428,736
format_disk:
	-rm -r $(DISK)
	dd if=/dev/zero of=$(DISK) bs=541428736 count=1

SEG         := -j .text -j .rodata -j .data
SIZE_KERNEL          := 499
SIZE_BUILTIN         := 50
BASE_SEC_BUILTIN_SH  := $(SIZE_KERNEL) + 1
BASE_BUILTIN_SH      := $(shell echo $$((  (($(BASE_SEC_BUILTIN_SH) - 2) * $(SIZE_SEC))  )))
END_BUILTIN_SH       := $(shell echo $$((  (($(BASE_SEC_BUILTIN_SH) + $(SIZE_BUILTIN) - 2) * $(SIZE_SEC) - 1)  )))

# objdump -S: disassemble the text segment in a source intermixed style
#         -D: disassemble all the segments
#         -j: specify segments to be generated
$(BOOT_IMG): bootsect kernel.elf sh.elf
	dd if=bootsect of=$(BOOT_IMG) bs=$(SIZE_SEC) count=1 conv=notrunc
	objcopy -S -O binary kernel.elf kernel
	dd if=kernel of=$(BOOT_IMG) bs=$(SIZE_SEC) count=$(SIZE_KERNEL) seek=1 conv=notrunc
	objcopy -S -O binary user/sh.elf user/sh
	dd if=user/sh of=$(BOOT_IMG) bs=$(SIZE_SEC) count=$(SIZE_BUILTIN) \
		seek=$(shell echo $$((  $(BASE_SEC_BUILTIN_SH) - 1  ))) conv=notrunc
	objdump $(SEG) -SD -m i386 kernel.elf > kernel.elf.dis
	objdump $(SEG) -SD -m i386 user/sh.elf >> kernel.elf.dis

# --oformat: output the pure binary format
# -e: entry is `_start` by default, but this option can specify other entrys
# -Ttext: the linked address always add this value
# -m: emulate a 32 bit compling environment when use 64 bit machine to compile 32 bit code
#     DETAILS IN https://stackoverflow.com/questions/19200333/architecture-of-i386-input-file-is-incompatible-with-i386x86-64
bootsect: ./boot/bootsect.o
	$(LD) --oformat binary -e _start -Ttext 0x7c00 -m elf_i386 $< -o $@

./boot/bootsect.o: ./boot/bootsect.S
	$(CC) $(CFLAGS) $< -o $@

kernel.elf: $(OBJS) $(OBJC)
	$(LD) $(LDFLAGS) -T kernel.ld $^ -o $@

sh.elf: user/builtin_shell.o user/user.o
	$(LD) -m elf_i386 -e main_shell -T user/builtin.ld $^ -o user/$@

CFLAGS += -D__BASE_BUILTIN_SH=$(BASE_BUILTIN_SH) \
	-D__END_BUILTIN_SH=$(END_BUILTIN_SH) \
	-D__BASE_BUILTIN_PWD=$(BASE_BUILTIN_PWD) \
	-D__END_BUILTIN_PWD=$(END_BUILTIN_PWD) \
	-D__BASE_BUILTIN_LS=$(BASE_BUILTIN_LS) \
	-D__END_BUILTIN_LS=$(END_BUILTIN_LS) \
	-D__BASE_BUILTIN_CD=$(BASE_BUILTIN_CD) \
	-D__END_BUILTIN_CD=$(END_BUILTIN_CD) \
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

# -rm: some maybe not exist but we dont care
clean:
	-rm -r $(OBJS) $(OBJC) ./boot/bootsect.o bootsect \
	./kernel ./kernel.elf ./*.dis ./kernel.map $(BOOT_IMG) \
	user/*.elf user/sh
