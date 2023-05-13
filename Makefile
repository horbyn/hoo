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
CFLAGS := -c -Wall -Werror -m32 -nostdinc -fno-builtin -fno-pie -fno-stack-protector
# -m: specify the output format
# -Map: output memory map
LDFLAGS := -m elf_i386 -Map kernel.map

# keep the depencement `.img` in the first command pleaze, that `make` will
#    execute all the command by default
nop: clean image $(OBJS) $(OBJC) fd1_44M.img
	DISPLAY=:0 /usr/bin/bochs

debug: CFLAGS += -g -DDEBUG
debug: clean image $(OBJS) $(OBJC) fd1_44M.img
	DISPLAY=:0 /usr/bin/bochs

run:
	DISPLAY=:0 /usr/bin/bochs

# -f: dont generate the file if exists
image:
	if [ ! -f "fd1_44M.img" ]; then \
		dd if=/dev/zero of=fd1_44M.img bs=1474560 count=1; \
	fi

# objdump -S: disassemble the text segment in a source intermixed style
#         -D: disassemble all the segments
fd1_44M.img: bootsect kernel.elf
	dd if=bootsect of=fd1_44M.img bs=512 count=1 conv=notrunc
	objcopy -S -O binary kernel.elf kernel
	dd if=kernel of=fd1_44M.img bs=512 count=896 seek=1 conv=notrunc
	objdump -SD -m i386 kernel.elf > kernel.elf.d

# --oformat: output the pure binary format
# -e: entry is `_start` by default, but this option can specify other entrys
# -Ttext: the linked address always add this value
# -m: emulate a 32 bit compling environment when use 64 bit machine to compile 32 bit code
#     DETAILS IN https://stackoverflow.com/questions/19200333/architecture-of-i386-input-file-is-incompatible-with-i386x86-64
bootsect: ./boot/bootsect.o
	$(LD) --oformat binary -e _start -Ttext 0x7c00 -m elf_i386 $< -o $@

./boot/bootsect.o: ./boot/bootsect.s
	$(AS) --32 $< -I boot -o $@

kernel.elf: $(OBJS) $(OBJC)
	$(LD) $(LDFLAGS) -T kernel.ld $^ -o $@

# `OBJS` depand all the `.s(.c)`
# --32: generate 32 bit code implied the target is `Intel i386`
#       DETAILS IN https://sourceware.org/binutils/docs/as/i386_002dOptions.html#i386_002dOptions
$(OBJS): %.o: %.s
	$(AS) --32 $< -o $@

$(OBJC): %.o: %.c
	$(CC) $(CFLAGS) $< -o $@

# -rm: some maybe not exist but we dont care
clean:
	-rm -r $(OBJS) $(OBJC) ./boot/bootsect.o bootsect \
	./kernel ./kernel.elf ./kernel.elf.d ./kernel.map fd1_44M.img
