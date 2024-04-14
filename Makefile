BOOT_IMG := fd1_44M.img
DISK     := hd.img

# SSRC: the `.s` set, which is used the `find` command to find out all the `.s`
#       in current dir and all the recursive sub-dir
# CSRC: the `.c` set
# OBJS: combine `SSRC` and `CSRC` to be the `.o` set, then filters `bootsect.o`
#       DETIALS IN https://seisman.github.io/how-to-write-makefile/functions.html
#       https://stackoverflow.com/questions/33740270/makefile-patsubst-multiple-expressions
SSRC := $(shell find . -name "*.s")
CSRC := $(shell find . -name "*.c")
OBJS := $(filter-out ./boot/bootsect.o, $(SSRC:.s=.o))
OBJC := $(CSRC:.c=.o)

AS := as
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
	-nostdinc -fno-builtin -fno-pie -fno-stack-protector
# -m: specify the output format
# -Map: output memory map
LDFLAGS := -m elf_i386 -Map kernel.map

# keep the depencement `.img` in the first command pleaze, that `make` will
#    execute all the command by default
nop: boot_image $(OBJS) $(OBJC) $(BOOT_IMG) run

debug: CFLAGS += -g -DDEBUG
debug: boot_image $(OBJS) $(OBJC) $(BOOT_IMG) run

test: CFLAGS += -DTEST
test: debug

run:
	bochs -q

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

# objdump -S: disassemble the text segment in a source intermixed style
#         -D: disassemble all the segments
#         -j: specify segments to be generated
$(BOOT_IMG): bootsect kernel.elf
	dd if=bootsect of=$(BOOT_IMG) bs=512 count=1 conv=notrunc
	objcopy -S -O binary kernel.elf kernel
	dd if=kernel of=$(BOOT_IMG) bs=512 count=896 seek=1 conv=notrunc
	objdump -j .text -j .rodata -j .data -SD -m i386 kernel.elf > kernel.elf.dis

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

$(OBJS): %.o: %.s
	$(CC) $(CFLAGS) $< -o $@

$(OBJC): %.o: %.c
	$(CC) $(CFLAGS) $< -o $@

# -rm: some maybe not exist but we dont care
clean:
	-rm -r $(OBJS) $(OBJC) ./boot/bootsect.o bootsect \
	./kernel ./kernel.elf ./kernel.elf.dis ./kernel.map $(BOOT_IMG)
